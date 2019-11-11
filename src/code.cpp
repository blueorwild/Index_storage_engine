#include "../include/code.h"

void Int64ToStr(int64_t num, char *str){
    int subs = 0;
    for(int i = 7; i >= 0; --i)
        str[subs++] = (unsigned char)(num >> 8*i);
}
int64_t StrToInt64(char *str){
    int64_t num = 0;
    num |= *str++;
    for(int i = 1; i < 8 ; ++i)
        num = (num << 8) | (unsigned char)(*str++);
    return num;
}

void IntToStr(int num, char *str){
    int subs = 0;
    for(int i = 3; i >= 0; --i)
        str[subs++] = (unsigned char)(num >> 8*i);
}
int StrToInt(char *str){
    int num = 0;
    num |= *str++;
    for(int i = 1; i < 4 ; ++i)
        num = (num << 8) | (unsigned char)(*str++);
    return num;
}
