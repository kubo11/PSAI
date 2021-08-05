#ifndef PSAI_EXECUTION_H
#define PSAI_EXECUTION_H

#include "data.h"

int get_sign_PSR(char *PSR);
int get_sign_number(int n);
int check_jump_conditions(char c, int sign);
void update_PSR(char (*PSR)[], int sign, int addr);
int step(memory_array **tab_m, instruction_array **tab_i, reg (*registers)[], char (*PSR)[], int *i);
int execute(memory_array **tab_m, instruction_array **tab_i, reg (*registers)[]);
int min(int a, int b);
int add(int* x, int y, char(*PSR)[], instruction_array tab_i, int* i, int memory_offset);
int subtract(int* x, int y, char(*PSR)[], instruction_array tab_i, int* i, int memory_offset);
int multiply(int* x, int y, char(*PSR)[], instruction_array tab_i, int* i, int memory_offset);
int divide(int* x, int y, char(*PSR)[], instruction_array tab_i, int* i, int memory_offset);
int jump(char jump_type, int sign, char value_type, int address, int* i, int size, instruction_array tab_i, char(*PSR)[], int memory_offset);
int compare(int x, int y, char(*PSR)[], instruction_array tab_i, int* i, int memory_offset);
int store(int* x, char* is_constant, int y, char(*PSR)[], instruction_array tab_i, int* i, int memory_offset);
int load(int* x_value, char* x_type, int y_value, char y_type, char(*PSR)[], instruction_array tab_i, int* i, int memory_offset);

#endif //PSAI_EXECUTION_H
