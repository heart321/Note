#include<stdio.h>
#include<stdlib.h>

void swap(int* a,int* b)
{
	int temp = *a;
	*a = *b ;
	*b = temp;
	
}

int partition(int* arr,int low,int high)
{
	int privot = arr[high];
	int i = (low) - 1;
	
	for(int j = low;j <= high - 1;j++)
	{
		if(arr[j] <= privot)
		{
			i++;
			swap(&arr[i],&arr[j]);
		}
	}
	
	swap(&arr[i+1],&arr[high]);
	return (i+1);
	
}

void quick_sort(int* arr,int low,int high)
{
	if(low < high)
	{
		int pi = partition(arr,low,high);
		
		//排序左边
		quick_sort(arr,low,pi-1);
		
		//排序右边
		quick_sort(arr,pi+1,high);
	}
}


int main(int argc, char const *argv[])
{
    int arr[] = {5,4,3,2,1};
    quick_sort(arr,0,4);
    for(int i = 0; i < 5; i++){
        printf("%d ",arr[i]);
    }
    return 0;
}
