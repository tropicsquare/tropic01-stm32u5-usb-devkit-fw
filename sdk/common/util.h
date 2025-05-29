#ifndef UTIL_H
#define UTIL_H

#include "common.h"

char strnicmp(const char *str1, const char *str2, unsigned char len);
bool is_char(char ch);
int is_number(const char *str, int limit);
bool is_hex(char ch);
int hex_to_bin(u8 *dest, const char *src, int limit);

#endif // ! UTIL_H

