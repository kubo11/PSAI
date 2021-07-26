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
int add(int* x, int y, char(*PSR)[], int PSR_address, int* i);
int subtract(int* x, int y, char(*PSR)[], int PSR_address, int* i);
int multiply(int* x, int y, char(*PSR)[], int PSR_address, int* i);
int divide(int* x, int y, char(*PSR)[], int PSR_address, int* i);
int jump(char jump_type, int sign, char value_sign, int address, int* i, int size, instruction_node tab[], char(*PSR)[], int PSR_address);
int compare(int x, int y, char(*PSR)[], int PSR_address, int* i);
int store(int* x, char* is_constant, int y, char(*PSR)[], int PSR_address, int* i);
int load(int* x_value, char *x_type, int y_value, char y_type, char(*PSR)[], int PSR_address, int* i);

#endif //PSAI_EXECUTION_H
