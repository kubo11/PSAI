#include <stdio.h>
#include "inout.h"
#include "data.h"
#include "execution.h"

#define path_length 256

int main(int argc, char **argv) {
    fileio files = { .in = NULL, .out = NULL, .mcsk = NULL, .exit_code = 0, .mode = '0' };
    errno_t err;
    memory_array *tab_m = NULL;
    instruction_array *tab_i = NULL;
    reg registers[16];
    int i, code;
    char path[path_length];
 
    if (code = check_input_arguments(argc, argv)) return cleanupnexit(code, files, tab_m, tab_i);
    if ((files = open_files(argc, argv)).exit_code) return cleanupnexit(files.exit_code, files, tab_m, tab_i);
    setup_arrays(&tab_m, &tab_i);
    setup_registers(&registers);
    if (files.mode == '0') {
        if (code = import_memory_psa(files.in, &tab_m)) return cleanupnexit(code, files, tab_m, tab_i);
        if (code = import_instructions_psa(files.in, &tab_i)) return cleanupnexit(code, files, tab_m, tab_i);
    }
    else {
        if (code = import_memory_mcsk(files.in, &tab_m)) return cleanupnexit(code, files, tab_m, tab_i);
        if (code = import_instructions_mcsk(files.in, &tab_i)) return cleanupnexit(code, files, tab_m, tab_i);
    }
    if (code = update_instructions(tab_m, &tab_i)) return cleanupnexit(code, files, tab_m, tab_i);
    if (files.mode == '0') {
        print_memory(files.mcsk, tab_m);
        print_instructions(files.mcsk, tab_i);
    }
    if (code = execute(&tab_m, &tab_i, &registers)) return cleanupnexit(code, files, tab_m, tab_i);
    print_memory(files.out, tab_m);
    print_instructions(files.out, tab_i);

    return cleanupnexit(0, files, tab_m, tab_i);
}