//
// Created by 15819 on 2025/3/18.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


//实现strcmp
int my_strcmp(char* str1, char* str2) {
    while (*str1 && *str2 && *str1 == *str2) {
        str1++;
        str2++;
    }
    return *str1 - *str2;
}

int main()
{
    int res = my_strcmp("hello1","hello");

    printf("res:%d\n",res);
    return 0;
}