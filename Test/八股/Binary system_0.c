#include <stdio.h>
#include <stdlib.h>

int func(int x)
{
    int count = 0;

    while (x + 1)
    {
        count++;
        x |= (x + 1);
    }
    return count;
}

int main(int argc, char const *argv[])
{
    printf("%d\n", func(25));
    return 0;
}
