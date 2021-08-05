#include <curses.h>
#include <string.h>
#include "debug.h"
#include "data.h"
#include "execution.h"
#include "conversion.h"

#define HEADER_Y 1
#define WINDOW_X 2
#define WINDOW_Y HEADER_Y + 2
#define GAP 2
#define INSTRUCTION_X WINDOW_X
#define INSTRUCTION_WIDTH 52
#define REGISTER_X INSTRUCTION_X + INSTRUCTION_WIDTH + GAP
#define REGISTER_WIDTH 15
#define REGISTER_LENGTH 16
#define MEMORY_X REGISTER_X + REGISTER_WIDTH + GAP
#define MEMORY_WIDTH 28
#define PSR_X MEMORY_X + MEMORY_WIDTH + GAP
#define PSR_WIDTH 23
#define PSR_LENGTH 1
#define LABEL_WIDTH 16
#define SIGN_Y 6
#define SIGN_X 123
#define SIGN_WIDTH 3
#define SIGN_LENGTH 3
#define CONTROLLS_HEADER_Y SIGN_Y + SIGN_LENGTH + 1
#define CONTROLLS_Y CONTROLLS_HEADER_Y + 2
#define CONTROLLS_X PSR_X
#define CONTROLLS_WIDTH PSR_WIDTH
#define CONTROLLS_LENGTH 6
#define EXIT_MESSAGE_Y 22
#define KEY_SPACE 32
#define KEY_ESCAPE 27

void debug(memory_array *tab_m, instruction_array *tab_i, reg registers[], char mode) {
	WINDOW** windows = NULL, ** current_window = NULL;
	char PSR[64];
	const char* headers[] = { "INSTRUCTIONS", "REGISTERS", "MEMORY", "PSR", "LAST OPERATION SIGN"};
	int i = 0, instruction_offset = 0, memory_offset = 0, code = 0, key, exit = '0', last = 0;

	initscr();
	noecho();
	keypad(stdscr, 1);
	curs_set(0);
	resize_term(30, 128);

	for (int i = 0; i < 64; i++) PSR[i] = '0';
	windows = initWindow(tab_m, tab_i, registers, &PSR, headers, tab_m->size);
	hilighting_on(windows, tab_m, tab_i, registers, PSR, i, instruction_offset, memory_offset);
	initControlls();
	current_window = windows[0];

	while (exit == '0' && (key = getch()) != KEY_ESCAPE) {
		switch (key) {
		case KEY_LEFT:
			if (tab_i->size > REGISTER_LENGTH) {
				current_window = windows[0];
				highlight_header(strlen(headers[0]), strlen(headers[2]), 'i');
			}
			break;
		case KEY_RIGHT:
			if (tab_i->size > REGISTER_LENGTH) {
				current_window = windows[2];
				highlight_header(strlen(headers[0]), strlen(headers[2]), 'm');
			}
			break;
		case KEY_UP:
			if (tab_i->size > REGISTER_LENGTH) {
				if (current_window == windows[0] && instruction_offset > 0)
					prefresh(current_window, --instruction_offset, 0, WINDOW_Y, INSTRUCTION_X, WINDOW_Y + min(tab_i->size, REGISTER_LENGTH) - 1, INSTRUCTION_X + INSTRUCTION_WIDTH - 1);
				if (current_window == windows[2] && memory_offset > 0)
					prefresh(current_window, --memory_offset, 0, WINDOW_Y, MEMORY_X, WINDOW_Y + min(tab_m->size, REGISTER_LENGTH) - 1, MEMORY_X + MEMORY_WIDTH - 1);
			}
			break;
		case KEY_DOWN:
			if (tab_i->size > REGISTER_LENGTH) {
				if (current_window == windows[0] && instruction_offset + min(tab_i->size, REGISTER_LENGTH) < tab_i->size)
					prefresh(current_window, ++instruction_offset, 0, WINDOW_Y, INSTRUCTION_X, WINDOW_Y + min(tab_i->size, REGISTER_LENGTH) - 1, INSTRUCTION_X + INSTRUCTION_WIDTH - 1);
				if (current_window == windows[2] && memory_offset + min(tab_m->size, REGISTER_LENGTH) < tab_m->size)
					prefresh(current_window, ++memory_offset, 0, WINDOW_Y, MEMORY_X, WINDOW_Y + min(tab_m->size, REGISTER_LENGTH) - 1, MEMORY_X + MEMORY_WIDTH - 1);
			}
			break;
		case KEY_SPACE:
			last = i;
			if (i < tab_i->size)
				if (!(code = step(&tab_m, &tab_i, registers, &PSR, &i))) {
					update(windows, tab_i, tab_m, registers, PSR, last, instruction_offset, memory_offset);
					if (i < tab_i->size) {
						hilighting_on(windows, tab_m, tab_i, registers, PSR, i, instruction_offset, memory_offset);
						autoscroll_instruction(i, &instruction_offset, tab_i->size, windows[0]);
						autoscroll_memory(i, &memory_offset, tab_m->size, tab_i, registers, windows[2]);
					}
					else exit = '1';
				}
				else exit = '1';
			else exit = '1';
			break;
		default:
			break;
		}
	}

	show_exit_message(code);
	getch();

	endwin();
}


WINDOW** initWindow(memory_array *tab_m, instruction_array *tab_i, reg registers[], char(*PSR)[], const char* headers[], int memory_length) {
	WINDOW** windows = (WINDOW**)malloc(5 * sizeof(WINDOW*));
	char a[17], b[5];

	windows[0] = newpad(tab_i->size, INSTRUCTION_WIDTH);
	windows[1] = newwin(REGISTER_LENGTH, REGISTER_WIDTH, WINDOW_Y, REGISTER_X);
	windows[2] = newpad(tab_m->size, MEMORY_WIDTH);
	windows[3] = newwin(PSR_LENGTH, PSR_WIDTH, WINDOW_Y, PSR_X);
	windows[4] = newwin(SIGN_LENGTH, SIGN_WIDTH, SIGN_Y, SIGN_X);

	if (tab_i->size > REGISTER_LENGTH) attron(A_STANDOUT);
	mvprintw(HEADER_Y, (INSTRUCTION_WIDTH - strlen(headers[0])) / 2 + INSTRUCTION_X, headers[0]);
	if (tab_i->size > REGISTER_LENGTH) attroff(A_STANDOUT);
	mvprintw(HEADER_Y, (REGISTER_WIDTH - strlen(headers[1])) / 2 + REGISTER_X, headers[1]);
	mvprintw(HEADER_Y, (MEMORY_WIDTH - strlen(headers[2])) / 2 + MEMORY_X, headers[2]);
	mvprintw(HEADER_Y, (PSR_WIDTH - strlen(headers[3])) / 2 + PSR_X, headers[3]);
	mvprintw(SIGN_Y + SIGN_LENGTH / 2, PSR_X, headers[4]);

	box_border(min(tab_i->size, REGISTER_LENGTH), INSTRUCTION_WIDTH, WINDOW_Y, INSTRUCTION_X);
	box_border(REGISTER_LENGTH, REGISTER_WIDTH, WINDOW_Y, REGISTER_X);
	box_border(min(tab_m->size, REGISTER_LENGTH), MEMORY_WIDTH, WINDOW_Y, MEMORY_X);
	box_border(PSR_LENGTH, PSR_WIDTH, WINDOW_Y, PSR_X);
	box_border(SIGN_LENGTH, SIGN_WIDTH, SIGN_Y, SIGN_X);

	refresh();

	for (int i = 0; i < tab_i->size; i++) {
		instruction_node tmp = tab_i->tab[i];
		if (tmp.command[1] == 'R')
			mvwprintw(windows[0], i, 0, "%-16s %-2s %-2d,%-16d  %-2s %c%c", tmp.label, tmp.command, tmp.arg1, tmp.arg2, encode(tmp.command), dec(tmp.arg1), dec(tmp.arg2));
		else if (tmp.arg_label[0] == '\0') {
			snprintf(a, 17, "%d(%d)", tmp.offset, tmp.arg2);
			snprintf(b, 5, "%04X", tmp.offset);
			mvwprintw(windows[0], i, 0, "%-16s %-2s %-2d,%-16s  %-2s %c%c %-5s", tmp.label, tmp.command, tmp.arg1, a, encode(tmp.command), dec(tmp.arg1), dec(tmp.arg2), add_gap(b));
		}
		else if (tmp.command[0] == 'J') {
			snprintf(b, 5, "%04X", tmp.offset);
			mvwprintw(windows[0], i, 0, "%-16s %-2s %-19s  %-2s %c%c %-5s", tmp.label, tmp.command, tmp.arg_label, encode(tmp.command), dec(tmp.arg1), dec(tmp.arg2), add_gap(b));
		}
		else {
			snprintf(b, 5, "%04X", tmp.offset);
			mvwprintw(windows[0], i, 0, "%-16s %-2s %-2d,%-16s  %-2s %c%c %-5s", tmp.label, tmp.command, tmp.arg1, tmp.arg_label, encode(tmp.command), dec(tmp.arg1), dec(tmp.arg2), add_gap(b));
		}
	}

	for (int i = 0; i < REGISTER_LENGTH; i++) {
		snprintf(a, 9, "%08X", registers[i].value);
		mvwprintw(windows[1], i, 0, "%-2d: %-11s", i, add_gap(a));
	}
	snprintf(a, 9, "%08X", registers[14].value + (memory_length - 1) * 4);
	mvwprintw(windows[1], 14, 0, "%-2d: %-11s", 14, add_gap(a));
	

	for (int i = 0; i < tab_m->size; i++) {
		memory_node tmp = tab_m->tab[i];
		char a[9];

		if (tmp.is_constant == '0') {
			mvwprintw(windows[2], i, 0, "%-16s ~~ ~~ ~~ ~~", tmp.label);
		}
		else {
			snprintf(a, 9, "%08X", tmp.value);
			mvwprintw(windows[2], i, 0, "%-16s %-11s", tmp.label, add_gap(a));
		}
	}

	strncpy_s(a, 17, &((*PSR)[48]), 16);
	mvwprintw(windows[3], 0, 0, "%s", add_gap(a));

	mvwprintw(windows[4], SIGN_LENGTH / 2, SIGN_WIDTH / 2, "%c", (*PSR)[4]);

	prefresh(windows[0], 0, 0, WINDOW_Y, INSTRUCTION_X, WINDOW_Y + min(tab_i->size, REGISTER_LENGTH) - 1, INSTRUCTION_X + INSTRUCTION_WIDTH - 1);
	wrefresh(windows[1]);
	prefresh(windows[2], 0, 0, WINDOW_Y, MEMORY_X, WINDOW_Y + min(tab_m->size, REGISTER_LENGTH) - 1, MEMORY_X + MEMORY_WIDTH - 1);
	wrefresh(windows[3]);
	wrefresh(windows[4]);

	return windows;
}

void box_border(int length, int width, int y, int x) {
	mvaddch(y - 1, x - 1, ACS_ULCORNER);
	mvaddch(y - 1, x + width, ACS_URCORNER);
	mvaddch(y + length, x - 1, ACS_LLCORNER);
	mvaddch(y + length, x + width, ACS_LRCORNER);
	for (int i = 0; i < length; i++) {
		mvaddch(y + i, x - 1, ACS_VLINE);
		mvaddch(y + i, x + width, ACS_VLINE);
	}
	for (int i = 0; i < width; i++) {
		mvaddch(y - 1, x + i, ACS_HLINE);
		mvaddch(y + length, x + i, ACS_HLINE);
	}
}

void highlight_header(int instruction_header_length, int memory_header_length, char which) {
	if (which == 'i') {
		mvchgat(HEADER_Y, (INSTRUCTION_WIDTH - instruction_header_length) / 2 + INSTRUCTION_X, instruction_header_length, A_STANDOUT, 0, NULL);
		mvchgat(HEADER_Y, (MEMORY_WIDTH - memory_header_length) / 2 + MEMORY_X, memory_header_length, A_NORMAL, 0, NULL);
	}
	else {
		mvchgat(HEADER_Y, (INSTRUCTION_WIDTH - instruction_header_length) / 2 + INSTRUCTION_X, instruction_header_length, A_NORMAL, 0, NULL);
		mvchgat(HEADER_Y, (MEMORY_WIDTH - memory_header_length) / 2 + MEMORY_X, memory_header_length, A_STANDOUT, 0, NULL);
	}
	refresh();
}

void update(WINDOW** windows, instruction_array *tab_i, memory_array* tab_m, reg registers[], char PSR[], int line, int instruction_offset, int memory_offset) {
	char a[17];

	mvwchgat(windows[0], line, 0, INSTRUCTION_WIDTH, A_NORMAL, 0, NULL);

	wattron(windows[1], A_NORMAL);
	if (tab_i->tab[line].command[0] != 'J') {
		snprintf(a, 9, "%08X", registers[tab_i->tab[line].arg1].value);
		mvwprintw(windows[1], tab_i->tab[line].arg1, 0, "%-2d: %-11s", tab_i->tab[line].arg1, add_gap(a));
	}
	if (tab_i->tab[line].command[1] == 'R') {
		snprintf(a, 9, "%08X", registers[tab_i->tab[line].arg2].value);
		mvwprintw(windows[1], tab_i->tab[line].arg2, 0, "%-2d: %-11s", tab_i->tab[line].arg2, add_gap(a));
	}
	wattroff(windows[1], A_NORMAL);

	wattron(windows[2], A_NORMAL);
	if (tab_i->tab[line].command[0] != 'J' && tab_i->tab[line].command[1] != 'R') {
		mvwchgat(windows[2], (registers[tab_i->tab[line].arg2].value + tab_i->tab[line].offset) / 4, 0, LABEL_WIDTH + 1, A_NORMAL, 0, NULL);
		if (tab_m->tab[(registers[tab_i->tab[line].arg2].value + tab_i->tab[line].offset) / 4].is_constant == '1') {
			snprintf(a, 9, "%08X", tab_m->tab[(registers[tab_i->tab[line].arg2].value + tab_i->tab[line].offset) / 4].value);
			mvwprintw(windows[2], (registers[tab_i->tab[line].arg2].value + tab_i->tab[line].offset) / 4, LABEL_WIDTH + 1, "%s", add_gap(a));
		}
		else {
			mvwprintw(windows[2], (registers[tab_i->tab[line].arg2].value + tab_i->tab[line].offset) / 4, LABEL_WIDTH + 1, "~~ ~~ ~~ ~~");
		}
	}
	wattroff(windows[2], A_NORMAL);

	strncpy_s(a, 17, &PSR[48], 16);
	mvwprintw(windows[3], 0, 0, "%s", add_gap(a));

	mvwprintw(windows[4], SIGN_LENGTH / 2, SIGN_WIDTH / 2, "%c", PSR[4]);

	prefresh(windows[0], instruction_offset, 0, WINDOW_Y, INSTRUCTION_X, WINDOW_Y + min(tab_i->size, REGISTER_LENGTH) - 1, INSTRUCTION_X + INSTRUCTION_WIDTH - 1);
	wrefresh(windows[1]);
	prefresh(windows[2], memory_offset, 0, WINDOW_Y, MEMORY_X, WINDOW_Y + min(tab_m->size, REGISTER_LENGTH) - 1, MEMORY_X + MEMORY_WIDTH - 1);
	wrefresh(windows[3]);
	wrefresh(windows[4]);
}

void hilighting_on(WINDOW** windows, memory_array* tab_m, instruction_array* tab_i, reg registers[], char PSR[], int line, int instruction_offset, int memory_offset) {
	mvwchgat(windows[0], line, 0, INSTRUCTION_WIDTH, A_STANDOUT, 0, NULL);
	if (tab_i->tab[line].command[0] != 'J')
		mvwchgat(windows[1], tab_i->tab[line].arg1, 0, REGISTER_WIDTH, A_STANDOUT, 0, NULL);
	if (tab_i->tab[line].command[1] == 'R')
		mvwchgat(windows[1], tab_i->tab[line].arg2, 0, REGISTER_WIDTH, A_STANDOUT, 0, NULL);
	if (tab_i->tab[line].command[0] != 'J' && tab_i->tab[line].command[1] != 'R')
		mvwchgat(windows[2], (registers[tab_i->tab[line].arg2].value + tab_i->tab[line].offset) / 4, 0, MEMORY_WIDTH, A_STANDOUT, 0, NULL);

	prefresh(windows[0], instruction_offset, 0, WINDOW_Y, INSTRUCTION_X, WINDOW_Y + min(tab_i->size, REGISTER_LENGTH) - 1, INSTRUCTION_X + INSTRUCTION_WIDTH - 1);
	wrefresh(windows[1]);
	prefresh(windows[2], memory_offset, 0, WINDOW_Y, MEMORY_X, WINDOW_Y + min(tab_m->size, REGISTER_LENGTH) - 1, MEMORY_X + MEMORY_WIDTH - 1);
	wrefresh(windows[3]);
}

void initControlls() {
	WINDOW* window = newwin(CONTROLLS_LENGTH, CONTROLLS_WIDTH, CONTROLLS_Y, CONTROLLS_X);
	const char* messages[] = { "SPACE - next instruct", "UP - scroll up", "DOWN - scroll down", "LEFT - select instruct", "RIGHT - select memory", "ESC - exit" };
	const char* header = "CONTROLLS";
	mvprintw(CONTROLLS_HEADER_Y, (CONTROLLS_WIDTH - strlen(header)) / 2 + CONTROLLS_X, header);
	box_border(CONTROLLS_LENGTH, CONTROLLS_WIDTH, CONTROLLS_Y, CONTROLLS_X);
	for (int i = 0; i < CONTROLLS_LENGTH; i++)
		mvwprintw(window, i, 0, messages[i]);
	refresh();
	wrefresh(window);
}

void autoscroll_instruction(int i, int *instruction_offset, int instruction_length, WINDOW* window) {
	while (i < *instruction_offset && *instruction_offset > 0) (*instruction_offset)--;
	while (i >= *instruction_offset + min(instruction_length, REGISTER_LENGTH) && *instruction_offset < instruction_length) (*instruction_offset)++;
	prefresh(window, *instruction_offset, 0, WINDOW_Y, INSTRUCTION_X, WINDOW_Y + min(instruction_length, REGISTER_LENGTH) - 1, INSTRUCTION_X + INSTRUCTION_WIDTH - 1);
}

void autoscroll_memory(int i, int *memory_offset, int memory_length, instruction_array *tab_i, reg registers[], WINDOW* window) {
	if (tab_i->tab[i].command[0] != 'J' && tab_i->tab[i].command[1] != 'R') {
		while ((registers[tab_i->tab[i].arg2].value + tab_i->tab[i].offset) /4 < *memory_offset && *memory_offset > 0) (*memory_offset)--;
		while ((registers[tab_i->tab[i].arg2].value + tab_i->tab[i].offset) / 4 >= *memory_offset + min(memory_length, REGISTER_LENGTH) && *memory_offset < memory_length) (*memory_offset)++;
		prefresh(window, *memory_offset, 0, WINDOW_Y, MEMORY_X, WINDOW_Y + min(memory_length, REGISTER_LENGTH) - 1, MEMORY_X + MEMORY_WIDTH - 1);
	}
}

void show_exit_message(int code) {
	if (code)
		mvprintw(EXIT_MESSAGE_Y, WINDOW_X, code_to_msg(code)), " (press any key to exit)";
	else
		mvprintw(EXIT_MESSAGE_Y, WINDOW_X, "PROGRAM ENDED SUCCESSFULLY (press any key to exit)");
}