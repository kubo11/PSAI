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
#define INSTRUCTION_X_PSA WINDOW_X
#define INSTRUCTION_X_MCSK WINDOW_X
#define INSTRUCTION_WIDTH_PSA 52
#define INSTRUCTION_WIDTH_MCSK 14
#define REGISTER_X_PSA INSTRUCTION_X_PSA + INSTRUCTION_WIDTH_PSA + GAP
#define REGISTER_X_MCSK INSTRUCTION_X_MCSK + INSTRUCTION_WIDTH_MCSK + GAP
#define REGISTER_WIDTH 17
#define REGISTER_LENGTH 16
#define MEMORY_X_PSA REGISTER_X_PSA + REGISTER_WIDTH + GAP
#define MEMORY_X_MCSK REGISTER_X_MCSK + REGISTER_WIDTH + GAP
#define MEMORY_WIDTH 24
#define PSR_X_PSA MEMORY_X_PSA + MEMORY_WIDTH + GAP
#define PSR_X_MCSK MEMORY_X_MCSK + MEMORY_WIDTH + GAP
#define PSR_WIDTH 25
#define PSR_LENGTH 1

void debug(memory_array *tab_m, instruction_array *tab_i, reg *registers, char mode) {
	WINDOW** windows = NULL, ** current_window = NULL;

	initscr();
	noecho();
	keypad(stdscr, 1);
	curs_set(0);
	resize_term(30, 128);

	if (mode == '0')
		windows = initPSA(tab_m, tab_i);
	else
		windows = initMCSK();
	current_window = windows[0];

	getch();
	endwin();
}


WINDOW** initPSA(memory_array *tab_m, instruction_array *tab_i) {
	WINDOW** windows = (WINDOW**)malloc(4 * sizeof(WINDOW*));
	int instruction_length = tab_i->size, memory_length = tab_m->size;
	const char* headers[] = { "INSTRUCTIONS", "REGISTERS", "MEMORY", "PSR" };
	char a[17], b[5];

	windows[0] = newpad(instruction_length, INSTRUCTION_WIDTH_PSA);
	windows[1] = newwin(REGISTER_LENGTH, REGISTER_WIDTH, WINDOW_Y, REGISTER_X_PSA);
	windows[2] = newpad(memory_length, MEMORY_WIDTH);
	windows[3] = newwin(PSR_LENGTH, PSR_WIDTH, WINDOW_Y, PSR_X_PSA);

	mvprintw(HEADER_Y, (INSTRUCTION_WIDTH_PSA - strlen(headers[0])) / 2 + INSTRUCTION_X_PSA, headers[0]);
	mvprintw(HEADER_Y, (REGISTER_WIDTH - strlen(headers[1])) / 2 + REGISTER_X_PSA, headers[1]);
	mvprintw(HEADER_Y, (MEMORY_WIDTH - strlen(headers[2])) / 2 + MEMORY_X_PSA, headers[2]);
	mvprintw(HEADER_Y, (PSR_WIDTH - strlen(headers[3])) / 2 + PSR_X_PSA, headers[3]);

	box_border(min(instruction_length, REGISTER_LENGTH), INSTRUCTION_WIDTH_PSA, WINDOW_Y, INSTRUCTION_X_PSA);
	box_border(REGISTER_LENGTH, REGISTER_WIDTH, WINDOW_Y, REGISTER_X_PSA);
	box_border(min(memory_length, REGISTER_LENGTH), MEMORY_WIDTH, WINDOW_Y, MEMORY_X_PSA);
	box_border(PSR_LENGTH, PSR_WIDTH, WINDOW_Y, PSR_X_PSA);

	refresh();

	prefresh(windows[0], 0, 0, WINDOW_Y, INSTRUCTION_X_PSA, WINDOW_Y + 15, INSTRUCTION_X_PSA + INSTRUCTION_WIDTH_PSA - 1);
	wrefresh(windows[1]);
	prefresh(windows[2], 0, 0, WINDOW_Y, MEMORY_X_PSA, WINDOW_Y + memory_length - 1, MEMORY_X_PSA + MEMORY_WIDTH - 1);
	wrefresh(windows[3]);

	for (int i = 0; i < instruction_length; i++) {
		instruction_node tmp = tab_i->tab[i];
		if (tmp.command[1] == 'R')
			mvwprintw(windows[0], i, 0, "%-16s %-2s %-2d,%-16d %s %c%c", tmp.label, tmp.command, tmp.arg1, tmp.arg2, encode(tmp.command), dec(tmp.arg1), dec(tmp.arg2));
		else if (tmp.arg_label[0] == '\0') {
			snprintf(a, 17, "%d(%d)", tmp.offset, tmp.arg2);
			snprintf(b, 5, "%04X", tmp.offset);
			mvwprintw(windows[0], i, 0, "%-16s %-2s %-2d,%-16s %s %c%c %c%c %c%c", tmp.label, tmp.command, tmp.arg1, a, encode(tmp.command), dec(tmp.arg1), dec(tmp.arg2), b[0], b[1], b[2], b[3]);
		}
		else if (tmp.command[0] == 'J') {
			snprintf(b, 5, "%04X", tmp.offset);
			mvwprintw(windows[0], i, 0, "%-16s %-2s %-19s %s %c%c %c%c %c%c", tmp.label, tmp.command, tmp.arg_label, encode(tmp.command), dec(tmp.arg1), dec(tmp.arg2), b[0], b[1], b[2], b[3]);
		}
		else {
			snprintf(b, 5, "%04X", tmp.offset);
			mvwprintw(windows[0], i, 0, "%-16s %-2s %-2d,%-16s %s %c%c %c%c %c%c", tmp.label, tmp.command, tmp.arg1, tmp.arg_label, encode(tmp.command), dec(tmp.arg1), dec(tmp.arg2), b[0], b[1], b[2], b[3]);
		}
	}
	prefresh(windows[0], 0, 0, WINDOW_Y, INSTRUCTION_X_PSA, WINDOW_Y + 15, INSTRUCTION_X_PSA + INSTRUCTION_WIDTH_PSA - 1);

	return windows;
}

WINDOW** initMCSK() {
	WINDOW** windows = (WINDOW**)malloc(4 * sizeof(WINDOW*));
	int instruction_length = 10, memory_length = 10, register_length = 16, PSR_length = 1;
	const char* headers[] = { "INSTRUCTIONS", "REGISTERS", "MEMORY", "PSR" };

	windows[0] = newwin(instruction_length + 2, INSTRUCTION_WIDTH_MCSK, WINDOW_Y, INSTRUCTION_X_MCSK);
	windows[1] = newwin(register_length + 2, REGISTER_WIDTH, WINDOW_Y, REGISTER_X_MCSK);
	windows[2] = newwin(memory_length + 2, MEMORY_WIDTH, WINDOW_Y, MEMORY_X_MCSK);
	windows[3] = newwin(PSR_length + 2, PSR_WIDTH, WINDOW_Y, PSR_X_MCSK);

	mvprintw(HEADER_Y, (INSTRUCTION_WIDTH_MCSK - strlen(headers[0])) / 2 + INSTRUCTION_X_MCSK, headers[0]);
	mvprintw(HEADER_Y, (REGISTER_WIDTH - strlen(headers[1])) / 2 + REGISTER_X_MCSK, headers[1]);
	mvprintw(HEADER_Y, (MEMORY_WIDTH - strlen(headers[2])) / 2 + MEMORY_X_MCSK, headers[2]);
	mvprintw(HEADER_Y, (PSR_WIDTH - strlen(headers[3])) / 2 + PSR_X_MCSK, headers[3]);

	refresh();

	for (int i = 0; i < 4; i++) {
		scrollok(windows[i], 1);
		box(windows[i], 0, 0);
		wrefresh(windows[i]);
	}

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