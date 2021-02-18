#ifndef __MY_STR_H__
#define __MY_STR_H__
#include <stdlib.h>
#include <unistd.h>
int my_strlen(char *str);
char *my_strcpy(char *dst, char *src);
char *my_strjoin(char *s1, char *s2);
void my_putstr_len(char *str);
void my_putchar(char c);
char *my_strchr(char *str, char c);
#endif