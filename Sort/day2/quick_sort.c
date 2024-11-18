#include<stdio.h>
#include<stdlib.h>

int partition(int *arr, int low, int high){
    int pivot = arr[low];
    while(low < high){
        while(arr[high] >= pivot && low < high){
            high--;
        }
        arr[low] = arr[high];
        while(arr[low] <= pivot && low < high){
            low++;
        }
        arr[high] = arr[low];
    }
    arr[low] = pivot;
    return low;

}



void quick_sort(int *arr, int low, int high){
    if(low < high){
        int pivot = partition(arr, low, high);
        quick_sort(arr,low,pivot-1);
        quick_sort(arr,pivot+1,high);
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
