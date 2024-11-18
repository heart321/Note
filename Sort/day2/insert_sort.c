#include <stdio.h>

void print_array(int *arr, int size)
{
    for (int i = 0; i < size; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void insert_sort(int *arr, int size)
{
    for (int i = 1; i < size; i++)
    {

        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key)
        {
            arr[j + 1] = arr[j]; // move the element to the right
            j--;
        }
        arr[j + 1] = key;
        print_array(arr, size);
    }
}

int main(int argc, char const *argv[])
{

    int arr[] = {5, 2, 4, 6, 1, 3};
    int size = sizeof(arr) / sizeof(arr[0]);
    insert_sort(arr, size);
    return 0;
}
