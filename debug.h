#pragma once
#include <curses.h>
#include "data.h"

void debug(memory_array* tab_m, instruction_array* tab_i, reg *registers, char mode);
WINDOW** initPSA();
WINDOW** initMCSK();
void box_border(int length, int width, int y, int x);
