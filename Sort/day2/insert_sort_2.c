#include<stdio.h>
#include<stdint.h>
#include<string.h>

void print_Arr(int* arr,int len)
{
	for(int i = 0;i < len;i++)
	{
		printf("%d ",arr[i]);
	}
	printf("\n");
}


void insertSort(int* arr,int len)
{
	for(int i = 1;i < len;i++)
	{
		int key = arr[i];
		int j = i - 1;
		while(j >= 0 && key < arr[j])
		{
			arr[j + 1] = arr[j];
			j--;
		}
		arr[j+1] = key;
		print_Arr(arr,len);
	}
}


int main(void)
{
	int arr[] = {1,2,5,7,123,5,123,5};
	int len = sizeof(arr) / sizeof(arr[0]);
	insertSort(arr,len);
	return 0;
}