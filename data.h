#ifndef PSAI_DATA_H
#define PSAI_DATA_H

#include <stdio.h>

struct instruction_node{
    char *label;
    char *command;
    int arg1;
    int arg2;
    int offset;
    char *arg_label;
    int address;
}typedef instruction_node;

struct instruction_array{
    struct instruction_node *tab;
    int size;
}typedef instruction_array;

struct memory_node{
    char *label;
    char is_constant;
    int value;
}typedef memory_node;

struct memory_array{
    struct memory_node *tab;
    int size;
}typedef memory_array;

struct reg{
    int value;
    char type;          // m - memory address, i - instruction address, v - value
}typedef reg;

struct fileio{
    FILE* in;
    FILE* out;
    FILE* mcsk;
    char mode;
    errno_t exit_code;
}typedef fileio;

int add_memory_node(memory_array **tab, char *label, char is_constant, int value);
int add_instruction_node(instruction_array **tab, char *label, char *command, int arg1, int arg2, int offset, char *arg_label);
int update_instructions(memory_array *tab_m, instruction_array **tab_i);
void delete_data(memory_array *tab_m, instruction_array *tab_i);
int setup_arrays(memory_array **tab_m, instruction_array **tab_i);
int setup_registers(reg (*registers)[], int memory_offset);
int bin_search(int p, int k, instruction_node tab[], int w);

#endif //PSAI_DATA_H
