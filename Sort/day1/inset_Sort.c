#include <stdio.h>

void insert_sort(int *arr, int len)
{
    for (int i = 1; i < len; i++)
    {
        int temp = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > temp)
        {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = temp;
    }
}

int main(int argc, char const *argv[])
{
    int a[5] = {1, 5, 6, 3, 2};

    insert_sort(a, 5);

    for (int i = 0; i < 5; i++)
    {
        printf("%d ", a[i]);
    }

    return 0;
}
/*
直接插入排序总结：

①元素越接近有序，直接插入排序的效率越高

②时间复杂度:O(N^2)

最坏的情况下，每次插入一个数字，前面的数字都要挪动一下，一共需要挪动1+2+3+……+n=n(n+1)/2

③空间复杂度：O(1)

没有借助额外的空间，只用到常数个变量

*/
