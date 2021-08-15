#ifndef PSAI_INOUT_H
#define PSAI_INOUT_H

#include <stdio.h>
#include "data.h"

void skip(char *buffer, int *i);
void read_string(char *buffer, int *i, char *var, int size);
void read_integer(char *buffer, int *i, int *number);
int import_memory_psa(FILE *input, memory_array **memory_tab);
int import_instructions_psa(FILE *input, instruction_array **instruction_tab);
int print_memory(FILE *output, memory_array *memory_tab);
int print_instructions(FILE *output, instruction_array *tab);
int import_memory_mcsk(FILE *input, memory_array **tab);
int import_instructions_mcsk(FILE *input, instruction_array **tab);
fileio open_files(int argc, char **argv);
int check_input_arguments(int argc, char** argv);
int close_files(fileio files, char mode);
int cleanupnexit(int code, fileio files, memory_array* tab_m, instruction_array* tab_i);

#endif //PSAI_INOUT_H
