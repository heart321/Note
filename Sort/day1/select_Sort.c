#include <stdio.h>

// 选择排序函数，参数为待排序数组arr和数组长度len
void select_sort(int *arr, int len) {
    int begin = 0, end = len - 1;
    // 循环遍历数组，直到begin小于end
    while (begin < end) {
        int min_index = begin;
        // 遍历begin到end之间的元素，找到最小的元素的下标
        for (int i = begin; i <= end; i++) {
            if (arr[i] < arr[min_index]) {
                min_index = i; // 遍历到最小的数据记录下来
            }
        }
        // 将begin位置的元素与最小元素交换
        int temp = arr[begin];
        arr[begin] = arr[min_index];
        arr[min_index] = temp; // 交换元素
        begin++; // 缩小范围
    }
}

int main(int argc, char const *argv[]) {
    int a[5] = {1, 5, 6, 3, 2};

    // 调用选择排序函数，对数组a进行排序
    select_sort(a, 5);

    // 遍历数组a，输出排序后的结果
    for (int i = 0; i < 5; i++) {
        printf("%d ", a[i]);
    }
    return 0;
}

