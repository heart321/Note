//
// Created by 15819 on 2025/3/14.
//
#include <stdio.h>

//49 38 65 97 76 13 27 49
void printArray(int* arr,int n);
void insert_arr(int* arr,int n);
void shell_arr(int* arr,int n);
void bobbleSort(int* arr,int n);
void selectedSort(int* arr,int n);
void quickSort(int* arr,int low,int high);
int partition(int *arr,int low,int high);
int binary_search(int * arr,int n,int target);


void swap(int* a,int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}
int main()
{
    int arr[] = {49,38,65,97,76,13,27,49};
    int n = sizeof(arr) / sizeof (arr[0]);
    printf("排序前的数组:");
    printArray(arr,n);

    //insert_arr(arr,n);
    //shell_arr(arr,n);
    //bobbleSort(arr,n);
    //quickSort(arr,0,n-1);
    selectedSort(arr,n);

    printf("排序后数组:");
    printArray(arr,n);

    int x = binary_search(arr,n,38);
    printf("找到的数：%d\n",x);


    return 0;
}

void printArray(int* arr,int n)
{
    for(int i = 0;i < n;++i)
    {
        printf("%d ",arr[i]);
    }
    printf("\n");
}
//49 38 65 97 76 13 27 49
void insert_arr(int* arr,int n)
{
    for(int i = 1;i < n;++i)
    {
        int temp = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > temp)
        {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j+1] = temp;
    }
}

//49 38 65 97 76 13 27 49
void shell_arr(int* arr,int n)
{
    for(int gap = n / 2;gap > 0; gap /= 2 )
    {
        for(int i = gap;i < n;++i)
        {
            int temp = arr[i];
            int j = i;
            while ( j >= gap && arr[j - gap] > temp)
            {
                arr[j] = arr[j - gap];
                j -= gap;
            }
            arr[j] = temp;
        }
    }
}

void bobbleSort(int* arr,int n)
{
    for(int i = 0;i < n-1;++i)
    {
        for(int j = 0;j < n-i-1;++j)
        {
            if(arr[j] > arr[j+1])
            {
                int temp = arr[j+1];
                arr[j+1] = arr[j];
                arr[j] = temp;
            }
        }
    }
}

int partition(int *arr,int low,int high)
{
    int pivot = arr[high];
    int i = low - 1;
    for(int j = low;j < high;++j)
    {
        if(arr[j] <= pivot)
        {
            ++i;
            swap(&arr[j],&arr[i]);
        }
    }
    swap(&arr[i+1],&arr[high]);
    return i+1;
}


//49 38 65 97 76 13 27 49
void quickSort(int* arr,int low,int high)
{
    if(low < high)
    {
        int pi = partition(arr,low,high);
        quickSort(arr,low,pi-1);
        quickSort(arr,pi+1,high);
    }
}

void selectedSort(int* arr,int n)
{
    for(int i = 0;i < n-1;++i)
    {
        int minIndex = i;
        for(int j = i+1;j <n;++j)
        {
            if(arr[j] < arr[minIndex])
            {
                minIndex = j;
            }
        }
        swap(&arr[i],&arr[minIndex]);
    }

}
// 二分查找
int binary_search(int * arr,int n,int target)
{
    int left = 0;
    int right = n - 1;
    while (left <= right)
    {
        int mid = left + (right - left) / 2;
        if(arr[mid] == target)
        {
            return mid;
        } else if(arr[mid] < target)
        {
            left = mid + 1;
        } else if(arr[mid]> target)
        {
            right = mid - 1;
        }
    }
    return -1;
}


