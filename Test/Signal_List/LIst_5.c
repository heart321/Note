#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct ListNode
{
    int val;
    struct ListNode *next;
};

// init list
struct ListNode *initList(void)
{
    struct ListNode *head = (struct ListNode *)malloc(sizeof(struct ListNode));
    head->next = NULL;
    head->val = -1;

    struct ListNode *pCurrent = head;

    int data = 0;
    printf("Please input data: (-1 end)");
    while (1)
    {
        scanf("%d", &data);
        if (data == -1)
        {
            break;
        }
        struct ListNode *node = (struct ListNode *)malloc(sizeof(struct ListNode));
        node->val = data;
        node->next = NULL;

        pCurrent->next = node;
        pCurrent = node;
    }

    return head;
}

// insert list
void insertList(struct ListNode *head, int olddata, int newdata)
{
    if (head == NULL)
    {
        return;
    }

    struct ListNode *pCurrent = head->next;
    struct ListNode *pPre = head;

    while (pCurrent != NULL)
    {
        if (pCurrent->val == olddata)
        {
            struct ListNode *node = (struct ListNode *)malloc(sizeof(struct ListNode));
            node->val = newdata;
            node->next = NULL;

            pPre->next = node;
            node->next = pCurrent;
            break;
        }
        pPre = pCurrent;
        pCurrent = pCurrent->next;
    }

    if (pCurrent == NULL)
    {
        struct ListNode *node = (struct ListNode *)malloc(sizeof(struct ListNode));
        node->val = newdata;
        node->next = NULL;

        pPre->next = node;
    }
}

// 修改链表中的某个值
void modifyList(struct ListNode *head, int olddata, int newdata)
{
    if (head == NULL)
    {
        return;
    }
    struct ListNode *pCurrent = head->next;
    while (pCurrent != NULL)
    {
        if (pCurrent->val == olddata)
        {
            pCurrent->val = newdata;
            break;
        }
        pCurrent = pCurrent->next;
    }
}

// delete list
void deleteList(struct ListNode *head, int data)
{
    if (head == NULL)
    {
        return;
    }

    struct ListNode *pCurrent = head->next;
    struct ListNode *pPre = head;

    while (pCurrent != NULL)
    {
        if (pCurrent->val == data)
        {
            pPre->next = pCurrent->next;
            free(pCurrent);
            break;
        }
        pPre = pCurrent;
        pCurrent = pCurrent->next;
    }
}

// print list
void printList(struct ListNode *head)
{
    struct ListNode *pCurrent = head->next;
    while (pCurrent != NULL)
    {
        printf("%d ", pCurrent->val);
        pCurrent = pCurrent->next;
    }
    printf("\n");
}

// 删除列表中的重复值
void deleteDuplicate(struct ListNode *head)
{
    if (head == NULL)
    {
        return;
    }

    struct ListNode *pCurrent = head;
    while (pCurrent != NULL)
    {
        struct ListNode *p = pCurrent;
        while (p->next != NULL)
        {
            if (p->next->val == pCurrent->val)
            {
                p->next = p->next->next;
            }
            else
            {
                p = p->next;
            }
        }
        pCurrent = pCurrent->next;
    }
    return head;
}

// 找出列表中倒数第k个节点
struct ListNode *findKthToTail(struct ListNode *head, int k)
{
    if(head == NULL)
    {
        return NULL;
    }

    struct ListNode *fast = head;
    struct ListNode *slow = head;

    for(int i = 0;i < k;i++)
    {
        if(fast == NULL)
        {
            return NULL;
        }
        fast = fast->next;
    }

    while(fast)
    {
        fast = fast->next;
        slow = slow->next;
    }

    return slow;
}

// 找出链表的中间节点
struct ListNode *findMiddleNode(struct ListNode *head)
{
    if(head == NULL)
    {
        return NULL;
    }

    struct ListNode *fast = head;
    struct ListNode *slow = head;
    
    while(fast && fast->next)
    {
        fast = fast->next->next;
        slow = slow->next;
    }

    return slow;
}

//反转链表(迭代)
struct ListNode *reverseList(struct ListNode *head)
{
    // if(head == NULL || head->next == NULL)
    // {
    //     return head;
    // }

    // struct ListNode *former = NULL;
    // struct ListNode *mid = head;
    // struct ListNode *later = NULL;

    // while(mid)
    // {
    //     //保存下一个节点
    //     later = mid->next;

    //     //反转节点
    //     mid->next = former;

    //     //移动指针
    //     former = mid;
    //     mid = later;
    // }
    // return former;

    //递归
    if(head == NULL || head->next == NULL)
    {
        return head;
    }
    struct ListNode *mid = head;
    struct ListNode *later = head->next;
    head = reverseList(later);
    later->next = mid;
    mid->next = NULL;
    return head;
    
}


int main(int argc, char const *argv[])
{
    struct ListNode *head = initList();
    printList(head);
    insertList(head, 1, 100);
    printList(head);
    return 0;
}
