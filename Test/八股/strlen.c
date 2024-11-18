#include<stdio.h>

int mylen(char *s)
{
    int i = 0;
    while(*s++ != '\0')
    {
        i++;

    }
    return i;
}