#pragma once
#include <curses.h>
#include "data.h"

void debug(memory_array* tab_m, instruction_array* tab_i, reg registers[], char mode);
WINDOW** initWindow(memory_array* tab_m, instruction_array* tab_i, reg registers[], char(*PSR)[], const char *headers[], int memory_length);
void box_border(int length, int width, int y, int x);
void highlight_header(int instruction_header_length, int memory_header_length, char which);
void update(WINDOW** windows, instruction_array* tab_i, memory_array* tab_m, reg registers[], char PSR[], int line, int instruction_offset, int memory_offset);
void hilighting_on(WINDOW** windows, memory_array* tab_m, instruction_array* tab_i, reg registers[], char PSR[], int line, int instruction_offset, int memory_offset);
void initControlls();
void autoscroll_instruction(int i, int *instruction_offset, int instruction_length, WINDOW* window);
void autoscroll_memory(int i, int* memory_offset, int memory_length, instruction_array* tab_i, reg registers[], WINDOW* window);
void show_exit_message(int code);