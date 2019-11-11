#ifndef _CODE_H_
#define _CODE_H_
#include <stdint.h>

// 8字节的整形字符串相互转换
extern void Int64ToStr(int64_t num, char *str);
extern int64_t StrToInt64(char *str);

// 十进制正整数和字符串的相互转换  88 <==> "88"
extern void UintToStr(uint32_t num, char* str);
extern uint32_t StrToUint(char* str);

// 4字节的整形字符串相互转换
extern void IntToStr(int num, char* str);
extern int StrToInt(char* str);
#endif // !_CODE_H_