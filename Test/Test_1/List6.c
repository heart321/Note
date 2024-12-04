#include<stdio.h>
#include<stdlib.h>

struct ListNode
{
    int val;
    struct ListNode *next;
};
// 普通算法实现两个升序链表的合并
struct ListNode* mergeTwoLists(struct ListNode* l1, struct ListNode* l2){
    // if(l1 == NULL) return l2;
    // if(l2 == NULL)  return l1;

    // struct ListNode *head = (struct ListNode*)malloc(sizeof(struct ListNode));
    // struct ListNode *prev = head;

    // while(l1 && l2)
    // {
    //     if(l1->val <= l2->val)
    //     {
    //         prev->next = l1;
    //         l1 = l1->next;
    //     }
    //     else
    //     {
    //         prev->next = l2;
    //         l2 = l2->next;
    //     }
    //     prev = prev->next;
    // }
    // prev->next = l1 ? l1 : l2;
    // return head->next;

    // 递归
    if(l1 == NULL) return l2;
    if(l2 == NULL) return l1;
    if(l1->val < l2->val)
    {
        l1->next = mergeTwoLists(l1->next,l2);
        return l1;
    }
    else
    {
        l2->next = mergeTwoLists(l1,l2->next);
        return l2;
    }
}