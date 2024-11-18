#include <stdio.h>

// 交换两个整数的值
void swap(int *a,int *b)
{
    int temp = *a; // 保存a的值
    *a = *b; // 将b的值赋给a
    *b = temp; // 将a的值赋给b
}

// 冒泡排序
void bubbling_sort(int *arr,int len)
{
    for(int i = 0;i < len;i++) // 外层循环控制排序的轮数
        for(int j = 0;j < len - i - 1;j++) // 内层循环控制每轮排序的比较次数
        {
            if(arr[j] > arr[j+1]) // 如果前一个数大于后一个数
            {
                swap(&arr[j],&arr[j+1]); // 交换两个数的位置
            }
        }

}


int main(int argc, char const *argv[])
{
    int a[5] = {1,5,6,3,2}; // 定义一个整型数组

    bubbling_sort(a,5); // 对数组进行冒泡排序

    for(int i =0;i < 5;i++) // 遍历数组
    {
        printf("%d ",a[i]); // 输出数组元素
    }
    return 0;
}
