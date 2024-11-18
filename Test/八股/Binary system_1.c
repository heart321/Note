#include<stdio.h>

int func(int x)
{
    int count = 0;
    while(x)
    {
        count++;
        x &= (x-1);
    }
    return count;
}


int main(int argc, char const *argv[])
{
    printf("%d\n",func(9999));
    return 0;
}

