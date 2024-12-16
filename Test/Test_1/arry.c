#include <stdio.h>
#include <string.h>
#include <stdlib.h>



// 最大子序和
int maxSubArray(int* nums, int numsSize)
{
    int max = nums[0];
    int sum = 0;
    for (int i = 0; i < numsSize; i++)
    {
        sum += nums[i];
        if (sum > max)
        {
            max = sum;
        }
        if (sum < 0)
        {
            sum = 0;
        }
    }
    return max;
}

//原地移除元素
int  removeElement(int* nums, int numsSize, int val)
{
    int count = 0;
    int *p = nums;
    for (int i = 0; i < numsSize; i++)
    {
        if (nums[i] != val)
        {
            *p = nums[i];
            p++;
            count++;
        }
    }
    return count;
}

//合并两个有序数组
void merge(int* nums1, int nums1Size, int m, int* nums2, int nums2Size, int n)
{
    int M = m - 1;
    int N = n - 1;
    int cur = m + n - 1;

    while (M >= 0 && N >= 0)
    {
        if(nums1[M] >= nums2[N])
        {
            nums1[cur--]  = nums1[M--];
        }
        else
        {
            nums1[cur--] = nums2[N--];
        }
    }
    while (N >= 0)
    {
        nums1[cur--] = nums2[N--];
    }
}

int main(int argc, char const *argv[])
{
    int nums[] = { -2,1,-3,4,-1,2,1,-5,4 };
    int numsSize = sizeof(nums) / sizeof(nums[0]);
    printf("%d\n", maxSubArray(nums, numsSize));
    

    return 0;
}





