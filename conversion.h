#ifndef PSAI_CONVERSION_H
#define PSAI_CONVERSION_H

const char* encode(char* c);
const char* decode(char* c);
const char* code_to_msg(int code);
int hex(char c);
int dec(int n);
char* add_gap(char* s);

#endif //PSAI_CONVERSION_H
