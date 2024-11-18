#include<stdio.h>


//bubbling sort
void bubble_sort(int arr[], int n)
{
    for(int i = 0;i < n-1;i++)
    {
        for(int j = 0;j < n-i-1;j++)
        {
            if(arr[j] > arr[j+1])
            {
                int temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
    }
}




int main(int argc, char const *argv[])
{
    int arr[] = {1,2,3,4,5,6,7,8,9,10};
    int n = sizeof(arr)/sizeof(arr[0]);
    bubble_sort(arr,n);
    for(int i = 0;i < n;i++)
    {
        printf("%d ",arr[i]);
    }

    return 0;
}
