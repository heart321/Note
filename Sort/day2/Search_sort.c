#include <stdio.h>
#include <string.h>


void swap(int *a,int *b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

void SelectSort(int arr[],int n)
{
	int i,j,minIndex;
	
	for(i = 0;i < n - 1;i++)
	{
		minIndex = i;
		
		// 内层循环遍历未排序的数据
		for(j = i+1;j < n;j++)
		{
			if(arr[j] < arr[minIndex])
			{
				minIndex = j;
			}
		}
		
		// 将找到的最小元素交换到已排序的区末尾
		if(minIndex != i)
		{
			swap(&arr[i],arr[minIndex]);
		}
	}
}

void printArray(int arr[], int size) {
    int i;
    for (i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main() {
    int arr[] = {64, 25, 12, 22, 11};
    int n = sizeof(arr)/sizeof(arr[0]);

    printf("原始数组：");
    printArray(arr, n);

    selectionSort(arr, n);

    printf("排序后数组：");
    printArray(arr, n);
    
    return 0;
}