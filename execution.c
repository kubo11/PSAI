#include "execution.h"
#include "data.h"
#include <stdio.h>
#include <limits.h>

int step(memory_array **tab_m, instruction_array **tab_i, reg (*registers)[], char (*PSR)[], int *i) {
    if ((*tab_i)->tab[(*i)].command[0] == 'J'){
        int address = (*registers)[(*tab_i)->tab[(*i)].arg2].value + (*tab_i)->tab[(*i)].offset;
        int size = (*tab_i)->size - 1;
        int PSR_address = (int)&((*tab_i)->tab[(*i)]);
        char jump_type = (*tab_i)->tab[(*i)].command[1];
        char value_type = (*registers)[(*tab_i)->tab[(*i)].arg2].type;
        return jump(jump_type, get_sign_PSR((*PSR)), value_type, address, i, size, (*tab_i)->tab, PSR, PSR_address);
    }
    if ((*tab_i)->tab[(*i)].command[0] == 'A'){
        if ((*tab_i)->tab[(*i)].command[1] == 'R'){
            return add(&(*registers)[(*tab_i)->tab[(*i)].arg1].value, (*registers)[(*tab_i)->tab[(*i)].arg2].value, PSR, (int)&((*tab_i)->tab[(*i)]), i);
        }else{
            return add(&(*registers)[(*tab_i)->tab[(*i)].arg1].value, (*tab_m)->tab[((*registers)[(*tab_i)->tab[(*i)].arg2].value + (*tab_i)->tab[(*i)].offset) / 4].value, PSR, (int)&((*tab_i)->tab[(*i)]), i);
        }
    }
    if ((*tab_i)->tab[(*i)].command[0] == 'C'){
        if ((*tab_i)->tab[(*i)].command[1] == 'R'){
            return compare((*registers)[(*tab_i)->tab[(*i)].arg1].value, (*registers)[(*tab_i)->tab[(*i)].arg2].value, PSR, (int)&((*tab_i)->tab[(*i)]), i);
        }else{
            return compare((*registers)[(*tab_i)->tab[(*i)].arg1].value, (*tab_m)->tab[((*registers)[(*tab_i)->tab[(*i)].arg2].value + (*tab_i)->tab[(*i)].offset) / 4].value, PSR, (int)&((*tab_i)->tab[(*i)]), i);
        }
    }
    if ((*tab_i)->tab[(*i)].command[0] == 'M'){
        if ((*tab_i)->tab[(*i)].command[1] == 'R'){
            return multiply(&(*registers)[(*tab_i)->tab[(*i)].arg1].value, (*registers)[(*tab_i)->tab[(*i)].arg2].value, PSR, (int)&((*tab_i)->tab[(*i)]), i);
        }else{
            return multiply(&(*registers)[(*tab_i)->tab[(*i)].arg1].value, (*tab_m)->tab[((*registers)[(*tab_i)->tab[(*i)].arg2].value + (*tab_i)->tab[(*i)].offset) / 4].value, PSR, (int)&((*tab_i)->tab[(*i)]), i);
        }
    }
    if ((*tab_i)->tab[(*i)].command[0] == 'D'){
        if ((*tab_i)->tab[(*i)].command[1] == 'R'){
            return divide(&(*registers)[(*tab_i)->tab[(*i)].arg1].value, (*registers)[(*tab_i)->tab[(*i)].arg2].value, PSR, (int)&((*tab_i)->tab[(*i)]), i);
        }else{
            return divide(&(*registers)[(*tab_i)->tab[(*i)].arg1].value, (*tab_m)->tab[((*registers)[(*tab_i)->tab[(*i)].arg2].value + (*tab_i)->tab[(*i)].offset) / 4].value, PSR, (int)&((*tab_i)->tab[(*i)]), i);
        }
    }
    if ((*tab_i)->tab[(*i)].command[0] == 'L'){
        if ((*tab_i)->tab[(*i)].command[1] == 'R'){
            return load(&(*registers)[(*tab_i)->tab[(*i)].arg1].value, &(*registers)[(*tab_i)->tab[(*i)].arg1].type, (*registers)[(*tab_i)->tab[(*i)].arg2].value, (*registers)[(*tab_i)->tab[(*i)].arg2].type, PSR, (int)&((*tab_i)->tab[(*i)]), i);
        }else if ((*tab_i)->tab[(*i)].command[1] == 'A'){
            return load(&(*registers)[(*tab_i)->tab[(*i)].arg1].value, &(*registers)[(*tab_i)->tab[(*i)].arg1].type, ((*registers)[(*tab_i)->tab[(*i)].arg2].value + (*tab_i)->tab[(*i)].offset), (*registers)[(*tab_i)->tab[(*i)].arg2].type, PSR, (int)&((*tab_i)->tab[(*i)]), i);
        }else{
            return load(&(*registers)[(*tab_i)->tab[(*i)].arg1].value, &(*registers)[(*tab_i)->tab[(*i)].arg1].type, (*tab_m)->tab[((*registers)[(*tab_i)->tab[(*i)].arg2].value + (*tab_i)->tab[(*i)].offset) / 4].value, 'v', PSR, (int)&((*tab_i)->tab[(*i)]), i);
        }
    }
    if ((*tab_i)->tab[(*i)].command[0] == 'S'){
        if ((*tab_i)->tab[(*i)].command[1] == 'R'){
            return subtract(&(*registers)[(*tab_i)->tab[(*i)].arg1].value, (*registers)[(*tab_i)->tab[(*i)].arg2].value, PSR, (int)&((*tab_i)->tab[(*i)]), i);
        }else if ((*tab_i)->tab[(*i)].command[1] == 'T'){
            return store(&(*tab_m)->tab[((*registers)[(*tab_i)->tab[(*i)].arg2].value + (*tab_i)->tab[(*i)].offset) / 4].value, &(*tab_m)->tab[((*registers)[(*tab_i)->tab[(*i)].arg2].value + (*tab_i)->tab[(*i)].offset) / 4].is_constant, (*registers)[(*tab_i)->tab[(*i)].arg1].value, PSR, (int)&((*tab_i)->tab[(*i)]), i);
        }else{
            return subtract(&(*registers)[(*tab_i)->tab[(*i)].arg1].value, (*tab_m)->tab[((*registers)[(*tab_i)->tab[(*i)].arg2].value + (*tab_i)->tab[(*i)].offset) / 4].value, PSR, (int)&((*tab_i)->tab[(*i)]), i);
        }
    }
    return 1;
}

int execute(memory_array **tab_m, instruction_array **tab_i, reg (*registers)[]){
    int i = 0;
    char PSR[64];

    for (i = 0; i < 64; i++)
        PSR[i] = '0';
    for (i = 0; i < (*tab_i)->size;)
        if (step(tab_m, tab_i, registers, &PSR, &i) == 1) return 1;
 
    return 0;
}

int get_sign_PSR(char *PSR){
    if (PSR[4] == '0') return 0;
    else if (PSR[4] == '4') return 1;
    else if (PSR[4] == '8') return -1;
    else return 2;
}

int get_sign_number(int n){
    if (n > 0)
        return 1;
    else if (n < 0)
        return -1;
    else return 0;
}

int check_jump_conditions(char c, int sign){
    if(c == '\0' || (c == 'P' && sign == 1) || (c == 'Z' && sign == 0) || (c == 'N' && sign == -1))
        return 1;
    else
        return 0;
}

void update_PSR(char (*PSR)[], int sign, int addr){
    char address[32];
    int i;

    snprintf(address, 32, "%032X", addr);
    if (sign == 0) (*PSR)[4] = '0';
    else if (sign == 1) (*PSR)[4] = '4';
    else if (sign == -1) (*PSR)[4] = '8';
    else (*PSR)[4] = 'C';
    for (i = 32; i < 64; i++)
        (*PSR)[i] = address[i - 32];
}

int min(int a, int b){
    if(a < b) return a;
    else return b;
}

int add(int *x, int y, char (*PSR)[], int PSR_address, int *i) {
    if ((*x) > 0 && y > INT_MAX - (*x)) {
        update_PSR(PSR, 2, PSR_address);
        return 1;
    }
    if ((*x) < 0 && y < INT_MIN - (*x)) {
        update_PSR(PSR, 2, PSR_address);
        return 1;
    }

    (*x) += y;
    update_PSR(PSR, get_sign_number(*x), PSR_address);
    (*i)++;
    return 0;
}

int subtract(int *x, int y, char(*PSR)[], int PSR_address, int* i) {
    if ((*x) > 0 && y < (*x) + INT_MIN) {
        update_PSR(PSR, 2, PSR_address);
        return 1;
    }
    if ((*x) < 0 && y > (*x) + INT_MAX) {
        update_PSR(PSR, 2, PSR_address);
        return 1;
    }

    (*x) -= y;
    update_PSR(PSR, get_sign_number(*x), PSR_address);
    (*i)++;
    return 0;
}

int multiply(int *x, int y, char(*PSR)[], int PSR_address, int* i) {
    if (((*x) == INT_MIN && y == -1) || ((*x) == -1 && y == INT_MIN)) {
        update_PSR(PSR, 2, PSR_address);
        return 1;
    }
    if ((*x) != 0) {
        if (y > INT_MAX / (*x)) {
            update_PSR(PSR, 2, PSR_address);
            return 1;
        }
        if (y < INT_MIN / (*x)) {
            update_PSR(PSR, 2, PSR_address);
            return 1;
        }
    }

    (*x) *= y;
    update_PSR(PSR, get_sign_number(*x), PSR_address);
    (*i)++;
    return 0;
}

int divide(int* x, int y, char(*PSR)[], int PSR_address, int* i) {
    if (y == 0) {
        update_PSR(PSR, 2, PSR_address);
        return 1;
    }
    if (((*x) == INT_MIN && y == -1) || ((*x) == -1 && y == INT_MIN)) {
        update_PSR(PSR, 2, PSR_address);
        return 1;
    }

    (*x) /= y;
    update_PSR(PSR, get_sign_number(*x), PSR_address);
    (*i)++;
    return 0;
}

int jump(char jump_type, int sign, char value_type, int address, int *i, int size, instruction_node tab[], char(*PSR)[], int PSR_address) {
    if (check_jump_conditions(jump_type, sign)) {
        if (value_type == 'i') {
            (*i) = bin_search(address / 4, min(address / 2, size), tab, address);
            if ((*i) < 0) {
                update_PSR(PSR, 2, PSR_address);
                return 1;
            }
            update_PSR(PSR, get_sign_PSR((*PSR)), PSR_address);
            return 0;
        }
        update_PSR(PSR, 2, PSR_address);
        return 1;
    }
    (*i)++;
    update_PSR(PSR, get_sign_PSR((*PSR)), PSR_address);
    return 0;
}

int compare(int x, int y, char(*PSR)[], int PSR_address, int* i) {
    if (x > 0 && y < x + INT_MIN) {
        update_PSR(PSR, 2, PSR_address);
        return 1;
    }
    if (x < 0 && y > x + INT_MAX) {
        update_PSR(PSR, 2, PSR_address);
        return 1;
    }

    update_PSR(PSR, get_sign_number(x-y), PSR_address);
    (*i)++;
    return 0;
}

int store(int *x, char *is_constant, int y, char(*PSR)[], int PSR_address, int* i) {
    (*x) = y;
    (*is_constant) = '1';
    update_PSR(PSR, get_sign_PSR((*PSR)), PSR_address);
    (*i)++;
    return 0;
}

int load(int *x_value, char *x_type, int y_value, char y_type, char(*PSR)[], int PSR_address, int* i) {
    (*x_value) = y_value;
    (*x_type) = y_type;
    update_PSR(PSR, get_sign_PSR((*PSR)), PSR_address);
    (*i)++;
    return 0;
}