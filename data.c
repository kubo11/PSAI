#include "data.h"
#include <stdlib.h>
#include <string.h>

int add_memory_node(memory_array **tab, char *label, char is_constant, int value){      //adds struct containing information about new variable to memory array
    memory_node* tmp = NULL;

    if ( (tmp = (memory_node*)realloc((*tab)->tab, ((*tab)->size + 1) * sizeof(memory_node))) == NULL )
        return 10;
    (*tab)->tab = tmp;
    if ( ((*tab)->tab[(*tab)->size].label = (char*)malloc(strlen(label)+1)) == NULL )
        return 10;
    strcpy_s((*tab)->tab[(*tab)->size].label, strlen(label)+1, label);
    (*tab)->tab[(*tab)->size].value = value;
    (*tab)->tab[(*tab)->size].is_constant = is_constant;
    (*tab)->size++;
    return 0;
}

int add_instruction_node(instruction_array **tab, char *label, char *command, int arg1, int arg2, int offset, char *arg_label){     //adds struct containing information about new instruction to instruction array
    instruction_node* tmp = NULL;

    if ( (tmp = (instruction_node*)realloc((*tab)->tab, ((*tab)->size + 1) * sizeof(instruction_node))) == NULL )
        return 10;
    (*tab)->tab = tmp;
    if ( ((*tab)->tab[(*tab)->size].label = (char*)malloc(strlen(label)+1)) == NULL )
        return 10;
    if ( ((*tab)->tab[(*tab)->size].command = (char*)malloc(strlen(command)+1)) == NULL )
        return 10;
    if ( ((*tab)->tab[(*tab)->size].arg_label = (char*)malloc(strlen(arg_label)+1)) == NULL )
        return 10;
    strcpy_s((*tab)->tab[(*tab)->size].label, strlen(label)+1, label);
    strcpy_s((*tab)->tab[(*tab)->size].command, strlen(command)+1, command);
    strcpy_s((*tab)->tab[(*tab)->size].arg_label, strlen(arg_label)+1, arg_label);
    (*tab)->tab[(*tab)->size].arg1 = arg1;
    (*tab)->tab[(*tab)->size].arg2 = arg2;
    (*tab)->tab[(*tab)->size].offset = offset;
    if ((*tab)->size > 0)
        (*tab)->tab[(*tab)->size].address = (*tab)->tab[(*tab)->size - 1].command[1] == 'R' ? 2 + (*tab)->tab[(*tab)->size - 1].address : 4 + (*tab)->tab[(*tab)->size - 1].address;
    else
        (*tab)->tab[(*tab)->size].address = 0;
    (*tab)->size++;
    return 0;
}

int update_instructions(memory_array *tab_m, instruction_array **tab_i){        //updates address information of instructions where label was supplied as an argument
    int i, j, offset = 0;
    char found = '0';

    for (i = 0; i < (*tab_i)->size; i++){
        if ((*tab_i)->tab[i].arg_label[0] != '\0'){
            if ((*tab_i)->tab[i].command[0] == 'J'){
                for (j = 0; j < (*tab_i)->size; j++){
                    if (strcmp((*tab_i)->tab[i].arg_label, (*tab_i)->tab[j].label) == 0){
                        (*tab_i)->tab[i].arg2 = 14;
                        (*tab_i)->tab[i].offset = offset;
                        found = '1';
                    }
                    if ((*tab_i)->tab[j].command[1] == 'R') offset += 2; else offset += 4;
                }
            }else{
                for (j = 0; j < tab_m->size; j++){
                    if (strcmp((*tab_i)->tab[i].arg_label, tab_m->tab[j].label) == 0){
                        (*tab_i)->tab[i].arg2 = 15;
                        (*tab_i)->tab[i].offset = j * 4;
                        found = '1';
                    }
                }
            }
            if (found == '0') return 4; else found = '0';
            offset = 0;
        }
    }
    return 0;
}

void delete_data(memory_array *tab_m, instruction_array *tab_i){        //deletes instruction and memory arrays
    free(tab_m->tab);
    free(tab_m);
    free(tab_i->tab);
    free(tab_i);
}

int setup_arrays(memory_array **tab_m, instruction_array **tab_i){      //allocates space for memory and instructions arrays, sets default values to NULL and 0
    if ( ((*tab_m) = (memory_array*)malloc(sizeof(memory_array))) == NULL ) return 10;
    (*tab_m)->size = 0;
    (*tab_m)->tab = NULL;
    if ( ((*tab_i) = (instruction_array*)malloc(sizeof(instruction_array))) == NULL ) return 10;
    (*tab_i)->size = 0;
    (*tab_i)->tab = NULL;
    return 0;
}

int setup_registers(reg (*registers)[]){        //gives every register value of zero and type value, assigns type instruction address to register 14 and memory address to register 15 
    int i;

    for (i = 0; i < 14; i++){
        (*registers)[i].type = 'v';
        (*registers)[i].value = 0;
    }
    (*registers)[14].type = 'i';
    (*registers)[14].value = 0;
    (*registers)[15].type = 'm';
    (*registers)[15].value = 0;
    return 0;
}

int bin_search(int p, int k, instruction_node tab[], int w){        //performs binary search by address on instruction array
    int s;
    while(p <= k){
        s = (p + k) / 2;
        if (tab[s].address > w)
            k = s - 1;
        else if (tab[s].address < w)
            p = s + 1;
        else return s;
    }
    return -1;
}