#include "conversion.h"
#include <string.h>
#include <stdlib.h>

const char* encode(char* c) {       //accepts command code and returns corresponding machine code
    int val;
    if (c[1] != ' ' && c[1] != '\0')
        val = (int)c[0] + (int)c[1];
    else
        val = (int)c[0];
    switch (val) {
        case 65:        //A
            return "D1";
        case 67:        //C
            return "D9";
        case 68:        //D
            return "D7";
        case 74:        //J
            return "E0";
        case 76:        //L
            return "F0";
        case 77:        //M
            return "D5";
        case 83:        //S
            return "D3";
        case 141:       //LA
            return "F2";
        case 147:       //AR
            return "10";
        case 149:       //CR
            return "18";
        case 150:       //DR
            return "16";
        case 152:       //JN
            return "E3";
        case 154:       //JP
            return "E2";
        case 158:       //LR
            return "31";
        case 159:       //MR
            return "14";
        case 164:       //JZ
            return "E1";
        case 165:       //SR
            return "12";
        case 167:       //ST
            return "F3";
        case 135:       //DC
            return "XC";
        case 151:       //DS
            return "XS";
        default:        //ERROR
            return "!";
    }
}

const char* decode(char* c) {       //accepts machine code and returns corresponding command code
    int val = (int)c[0] * 37 + (int)c[1];
    switch (val) {
        case 2565:        //A
            return "A";
        case 2573:        //C
            return "C";
        case 2571:        //D
            return "D";
        case 2601:        //J
            return "J";
        case 2638:        //L
            return "L";
        case 2569:        //M
            return "M";
        case 2567:        //S
            return "S";
        case 2640:       //LA
            return "LA";
        case 1861:       //AR
            return "AR";
        case 1869:       //CR
            return "CR";
        case 1867:       //DR
            return "DR";
        case 2604:       //JN
            return "JN";
        case 2603:       //JP
            return "JP";
        case 1936:       //LR
            return "LR";
        case 1865:       //MR
            return "MR";
        case 2602:       //JZ
            return "JZ";
        case 1863:       //SR
            return "SR";
        case 2641:       //ST
            return "ST";
        case 2583:       //DC
            return "XC";
        case 2599:       //DS
            return "XS";
        default:        //ERROR
            return "!";
    }
}

const char* code_to_msg(int code) {     //accepts error code and returns its message
    switch (code) {
    case 1:
        return "COULD NOT OPEN FILE";
    case 2:
        return "INVALID MEMORY SYNTAX";
    case 3:
        return "INVALID INSTRUCTION SYNTAX";
    case 4:
        return "LABEL DOESN'T EXIST";
    case 5:
        return "INT OVERFLOW";
    case 6:
        return "DIVISION BY ZERO";
    case 7:
        return "WRONG JUMP ADDRESS";
    case 8:
        return "COULD NOT CLOSE FILE";
    case 9:
        return "WRONG PROGRAM ARGUMENTS";
    case 10:
        return "OUT OF MEMORY";
    case 11:
        return "ACCESSING ADDRESS OUT OF BOUNDS";
    default:
        return "";
    }
}

int hex(char c) {       //converts char hex to int dec
    if (c <= '9')
        return (int)(c - 48);
    else
        return (int)(c - 55);
}

int dec(int n) {        //converts int dec to char hex
    if (n <= 9)
        return (char)(n + 48);
    else 
        return (char)(n + 55);
}

char* add_gap(char* s) {        //adds space after every even character of given string, returns empty string if inputs length is odd
    if (strlen(s) % 2 || strlen(s) <= 0) return "";

    int size = 3 * strlen(s) / 2 ;
    char* g = (char*)malloc(size * sizeof(char));
    if (!g) return "";
    for (int i = 0; i < strlen(s); i++) g[3 * i / 2] = s[i];
    for (int i = 2; i < size; i += 3) g[i] = ' ';
    g[size - 1] = '\0';
    
    return g;
}