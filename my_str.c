#include "my_str.h"

int my_strlen(char *str)
{
    int len = 0;
    while (str[len] != '\0')
        len++;

    return len;    
}

char *my_strcpy(char *dst, char *src)
{
    int len = 0;
    int index = 0;

    len = my_strlen(src);
    dst = (char*)malloc(sizeof(char) * len + 1);
    if (dst)
    {
        while(src[index])
        {
            dst[index] = src[index];
            index++;
        }
        dst[index] = '\0';
    }
    return dst;
}

char *my_strjoin(char *s1, char *s2)
{
    char *str = NULL;
    int len_s1 = 0;
    int len_s2 = 0;
    int i = 0;

    len_s1 = my_strlen(s1);
    len_s2 = my_strlen(s2);
    str = (char*)malloc(sizeof(char) * (len_s1 + len_s2) + 1);
    if (str)
    {
        while(*s1)
            str[i++] = *s1++;
        while(*s2)
            str[i++] = *s2++;
        str[i] = '\0';
    }    
    return str;
}

void my_putstr_len(char *str)
{
    int len = my_strlen(str);
    write(1, str, len);
}

void my_putchar(char c)
{
    write(1, &c, 1);
}

char *my_strchr(char *str, char c)
{
    int len = 0;
    char *ptr = str;
 
    len = my_strlen(str) + 1;
    while(len--)
    {
      if (*ptr == c)
        return ptr;
      ptr++;
    }
 
    return NULL;
}