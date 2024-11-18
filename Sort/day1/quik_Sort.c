#include<stdio.h>
#include<stdlib.h>


// 分区函数，用于快速排序
int partition(int *arr, int low, int high)
{
    // 将数组中的第一个元素作为基准值
    int pivot = arr[low];
    // 当low小于high时，继续循环
    while (low < high)
    {
        // 从high开始，找到第一个小于基准值的元素
        while(arr[high] >= pivot && low < high)
        {
            high--;
        }
        // 将找到的元素赋值给low位置的元素
        arr[low] = arr[high];

        // 从low开始，找到第一个大于基准值的元素
        while(arr[low] <= pivot && low < high)
        {
            low++;
        }
        // 将找到的元素赋值给high位置的元素
        arr[high] = arr[low];


    }
    // 将基准值赋值给low位置的元素
    arr[low] = pivot;
    // 返回low的位置
    return low;
    
}


// 快速排序函数
void quickSort(int *arr, int low, int high)
{
    // 当low小于high时，继续递归
    if(low < high)
    {
        // 获取分区后的基准值位置
        int pivot = partition(arr, low, high);
        // 对基准值左边的数组进行快速排序
        quickSort(arr, low, pivot-1);
        // 对基准值右边的数组进行快速排序
        quickSort(arr, pivot+1, high);
    }
}

int main(int argc, char const *argv[])
{
    // 定义一个数组
    int arr[] = {10, 7, 8, 9, 1, 5};

    // 对数组进行快速排序
    quickSort(arr, 0, 5);
    // 输出排序后的数组
    for(int i = 0; i < 6; i++)
    {
        printf("%d ", arr[i]);
    }
    return 0;
}
