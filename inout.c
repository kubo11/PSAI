#include "inout.h"
#include "data.h"
#include "conversion.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define buffer_size 128
#define label_size 16
#define command_size 3
#define path_length 256
#define extension_length 16
#define combined_length (path_length + 9)

int import_memory_psa(FILE *input, memory_array **tab){     //reads memory section from psa file
    char buffer[buffer_size], label[label_size], command[command_size];
    int i = 0, value = 0, amount = 0, code;

    while(fgets(buffer, buffer_size, input) != NULL){
        if(buffer[0] == '/')
            continue;
        if(buffer[0] == '\n' || buffer[0] == '\r')
            break;
        read_string(buffer, &i, label, label_size - 1);
        if (buffer[i] != ' ' && buffer[i] != '\t') return 2;
        skip(buffer, &i);
        read_string(buffer, &i, command, command_size - 1);
        strcpy_s(command, command_size, encode(command));
        if (command[0] != 'X') return 2;
        if (buffer[i] != ' ' && buffer[i] != '\t') return 2;
        skip(buffer, &i);
        if(buffer[i] >= '0' && buffer[i] <= '9')
            read_integer(buffer, &i, &amount);
        if(command[1] == 'C'){
            while(i < buffer_size && buffer[i] != '(') i++;
            i++;
            if (i < buffer_size && (buffer[i] >= '0' && buffer[i] <= '9' || buffer[i] == '-')) {
                read_integer(buffer, &i, &value);
                if (code = add_memory_node(tab, label, '1', value)) return code;
            } else {
                if (code = add_memory_node(tab, label, '1', rand())) return code;
                for (i = 0; i < amount - 1; i++)
                    if (code = add_memory_node(tab, "", '1', rand())) return code;
            }
        }else{
            if (code = add_memory_node(tab, label, '0', 0)) return code;
            for (i = 0; i < amount - 1; i++)
                if (code = add_memory_node(tab, "", '0', 0)) return code;
        }
        
        i = 0;
        value = 0;
        amount = 0;
        label[0] = '\0';
    }

    return 0;
}

int import_memory_mcsk(FILE *input, memory_array **tab){        //reads memory section from mcsk file
    char buffer[buffer_size];
    int num = 0, first, i, base = 1, code;

    while (fgets(buffer, buffer_size, input) != NULL){
        if (buffer[0] == '\n' || buffer[0] == '\r') return 2;
        if (buffer[0] == '~')
            if (code = add_memory_node(tab, "", '0', 0)) return code;
        else{
            first = hex(buffer[0]);
            num += ((first/8)%2 * (INT_MIN));
            num += ((first/4)%2 * 1073741824);
            num += ((first/2)%2 * 536870912);
            num += (first%2 * 268435456);
            for (i = 10; i > 0; i--){
                if (i%3 == 2) i--;
                num += hex(buffer[i])*base;
                base *= 16;
            }
            if (code = add_memory_node(tab, "", '1', num)) return code;
        }
    }
    return 0;
}

int import_instructions_psa(FILE *input, instruction_array **tab){      //reads instruction section from psa file
    char buffer[buffer_size], label[label_size], command[command_size], arg_label[label_size] = { '\0' };
    const char *check;
    int i = 0, arg1 = 0, arg2 = 0, offset = 0;

    while (fgets(buffer, buffer_size, input) != NULL){
        if (buffer[0] == '/')
            continue;
        if (buffer[0] == '\n' || buffer[0] == '\r')
            break;
        read_string(buffer, &i, label, label_size - 1);
        if (buffer[i] != ' ' && buffer[i] != '\t') return 3;
        skip(buffer, &i);
        read_string(buffer, &i, command, command_size - 1);
        check = encode(command);
        if (check[0] == 'X' || check[0] == '!') return 3;
        if (buffer[i] != ' ' && buffer[i] != '\t') return 3;
        skip(buffer, &i);
        if (buffer[i] >= 'A' && buffer[i] <= 'Z'){
            read_string(buffer, &i, arg_label, label_size - 1);
        }else if (buffer[i] >= '0' && buffer[i] <= '9'){
            read_integer(buffer, &i, &arg1);
            if (buffer[i] == ',') {
                i++;
                skip(buffer, &i);
                if (buffer[i] >= 'A' && buffer[i] <= 'Z')
                    read_string(buffer, &i, arg_label, label_size - 1);
                else if (buffer[i] >= '0' && buffer[i] <= '9') {
                    read_integer(buffer, &i, &arg2);
                    if (command[1] != 'R') {
                        if (buffer[i] == '(') {
                            i++;
                            offset = arg2;
                            read_integer(buffer, &i, &arg2);
                        }
                        else return 3;
                    }
                }
                else return 3;
            }
            else if (buffer[i] == '(') {
                i++;
                skip(buffer, &i);
                if (buffer[i] >= '0' && buffer[i] <= '9') {
                    read_integer(buffer, &i, &arg2);
                    offset = arg1;
                    arg1 = 0;
                }
            }
            else return 3;
        }else return 3;
        add_instruction_node(tab, label, command, arg1, arg2, offset, arg_label);
        i = 0;
        arg1 = 0;
        arg2 = 0;
        offset = 0;
        label[0] = '\0';
        arg_label[0] = '\0';
    }
    return 0;
}

int import_instructions_mcsk(FILE *input, instruction_array **tab){     //reads instruction section from mcsk file
    char buffer[buffer_size], command[command_size];
    int arg1, arg2, offset = 0;

    while (fgets(buffer, buffer_size, input) != NULL){
        if (buffer[0] == '\n' || buffer[0] == '\r') return 3;
        command[0] = buffer[0];
        command[1] = buffer[1];
        command[2] = '\0';
        strcpy_s(command, 3, decode(command));
        if (command[0] == '!' || command[0] == 'D') return 3;
        arg1 = hex(buffer[3]);
        arg2 = hex(buffer[4]);
        if (buffer[5] == ' ')
            offset = hex(buffer[6]) * 4096 + hex(buffer[7]) * 256 + hex(buffer[9]) * 16 + hex(buffer[10]);
        add_instruction_node(tab, "", command, arg1, arg2, offset, "");
    }
    return 0;
}

int print_memory(FILE *output, memory_array *tab){      //writes memory section to mcsk file
    int i;
    char hex[9];

    for (i = 0; i < tab->size; i++){
        if (tab->tab[i].is_constant == '1'){
            _snprintf_s(hex, 9, 9, "%08X", tab->tab[i].value);
            fprintf_s(output, "%c%c %c%c %c%c %c%c\n", hex[0], hex[1], hex[2], hex[3], hex[4], hex[5], hex[6], hex[7]);
        } else fprintf_s(output, "~~ ~~ ~~ ~~\n");
    }
    fprintf_s(output, "\n");
    return 0;
}

int print_instructions(FILE *output, instruction_array *tab){       //writes instruction section to mcsk file
    int i;
    char hex[5];

    for (i = 0; i < tab->size; i++){
        if (tab->tab[i].command[1] != 'R'){
            _snprintf_s(hex, 5, 5, "%04X", tab->tab[i].offset);
            fprintf_s(output, "%s %X%X %c%c %c%c\n", encode(tab->tab[i].command), tab->tab[i].arg1, tab->tab[i].arg2, hex[0], hex[1], hex[2], hex[3]);
        }else fprintf_s(output, "%s %X%X\n", encode(tab->tab[i].command), tab->tab[i].arg1, tab->tab[i].arg2);
    }
    return 0;
}

void skip(char *buffer, int *i){        //skpis any spaces and tabs in buffer
    while( buffer[(*i)] == ' ' || buffer[(*i)] == '\t' )
        (*i)++;
}

void read_string(char *buffer, int *i, char *var, int size){        //reads string form buffer until finds space, tab, new line, carrige return of file ends
    int j = 0;

    while( buffer[(*i)] != ' ' && buffer[(*i)] != '\t' && buffer[(*i)] != '\n' && buffer[(*i)] != '\r' && j < size ){
        var[j] = buffer[(*i)];
        j++;
        (*i)++;
    }
    var[j] = '\0';
}

void read_integer(char *buffer, int *i, int *number){       //reads integer form buffer until finds character other than decimal digit
    char negative = '0';

    (*number) = 0;
    if(buffer[(*i)] == '-'){
        negative = '1';
        (*i)++;
    }
    while( buffer[(*i)] >= '0' && buffer[(*i)] <= '9'){
        (*number) *= 10;
        (*number) += ((int)buffer[(*i)] - 48);
        (*i)++;
    }
    if(negative == '1')
        (*number) *= -1;
}

fileio open_files(int argc, char **argv) {      //opens needed input and output files, gives output files dynamic filenames, returns error if fails to open any file
    errno_t err;
    fileio files;
    int i = 0, length = 0, tmp;
    char path[combined_length], extension[extension_length];

    if (argc == 1 || (err = fopen_s(&files.in, argv[1], "r")) != 0)
        do {
            printf("Podaj sciezke do pliku: ");
            scanf_s("%s", path, path_length);
        } while (err = fopen_s(&files.in, path, "r"));
    else strcpy_s(path, path_length, argv[1]);
    
    length = strnlen_s(path, path_length);
    i = length - 1;
    while (i > 0 && path[i] != '.') i--;
    strcpy_s(extension, extension_length, &path[i]);
    path[i] = '\0';

    if (!(argc > 2 && strcmp(argv[2], "psa_code"))) {
        files.mode = '0';
        strcat_s(path, combined_length, "_mcsk");
        strcat_s(path, combined_length, extension);

        if ((err = fopen_s(&files.mcsk, path, "w")) != 0) {
            files.exit_code = err;
            return files;
        }
    }
    else files.mode = '1';

    path[i] = '\0';
    strcat_s(path, combined_length, "_out");
    strcat_s(path, combined_length, extension);

    if ((err = fopen_s(&files.out, path, "w")) != 0) {
        files.exit_code = err;
        return files;
    }

    files.exit_code = 0;
    return files;
}

int close_files(fileio files) {     //closes all opened files, returns error if it fails to do so
    if (files.in) 
        if (fclose(files.in)) return 8;
    if (files.out) 
        if (fclose(files.out)) return 8;
    if (files.mcsk)
        if (fclose(files.mcsk)) return 8;
    return 0;
}

int check_input_arguments(int argc, char** argv) {      //checks if program input arguments are valid
    if (argc > 4)
        return 9;
    if (argc > 2 && strcmp(argv[2], "psa_code") && strcmp(argv[2], "mcsk_code"))
        return 9;
    if (argc == 4 && strcmp(argv[3], "debug"))
        return 9;
    return 0;
}

int cleanupnexit(int code, fileio files, memory_array* tab_m, instruction_array* tab_i) {       //prints error message, deletes data, closes files and returns exit code
    printf("%s\n", code_to_msg(code));
    delete_data(tab_m, tab_i);
    if (close_files(files)) printf("%s\n", code_to_msg(8));
    return code;
}