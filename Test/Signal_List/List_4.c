#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct ListNode
{
    int data;
    struct ListNode *next;
};


// 初始化链表
struct ListNode *initList(void)
{
    struct ListNode *head = (struct ListNode *)malloc(sizeof(struct ListNode));
    head->data = -1;
    head->next = NULL;

    struct ListNode *p = head;

    int val = 0;
    printf("print the data of the list node(-1 to end):");

    while(1)
    {
        scanf("%d", &val);
        if(val == -1)
            break;

        struct ListNode *newNode = (struct ListNode *)malloc(sizeof(struct ListNode));
        newNode->data = val;
        newNode->next = NULL;

        // 插入节点
        p->next = newNode;
        p = p->next;
    }

    return head;
}

// 打印链表
void printList(struct ListNode *head)
{
    if(head == NULL)
        return;

    struct ListNode *p = head->next;
    while(p != NULL)
    {
        printf("%d ", p->data);
        p = p->next;
    }
    printf("\n");
}

// 插入节点
void insertNode(struct ListNode *head, int oldval,int newval)
{
    if(head == NULL)
        return;

    // two point
    struct ListNode *pCurrent = head->next;
    struct ListNode *pPre = head;

    while(pCurrent != NULL)
    {
        if(pCurrent->data == oldval)
        {
            struct ListNode *newNode = (struct ListNode *)malloc(sizeof(struct ListNode));
            newNode->data = newval;
            newNode->next = NULL;

            pPre->next = newNode;
            newNode->next = pCurrent;

            break;
        }
        pPre = pCurrent;
        pCurrent = pCurrent->next;
    }

    //if pCurrent == NULL
    if(pCurrent == NULL)
    {
        struct ListNode *newNode = (struct ListNode *)malloc(sizeof(struct ListNode));
        newNode->data = newval;
        newNode->next = NULL;

        pPre->next = newNode;

    }
}

// 删除节点
void deleteNode(struct ListNode *head, int val)
{
    if(head == NULL)
        return;

    struct ListNode *pCurrent = head->next;
    struct ListNode *pPre = head;

    while(pCurrent != NULL)
    {
        if(pCurrent->data == val)
        {
            pPre->next = pCurrent->next;
            free(pCurrent); // free the memory
            break;
        }
        pPre = pCurrent;
        pCurrent = pCurrent->next;
    }
}

//改变节点的值
void changeNode(struct ListNode *head, int oldval, int newval)
{
    if(head == NULL)
        return;
    
    struct ListNode *pCurrent = head->next;
    
    while(pCurrent != NULL)
    {
        if(pCurrent->data == oldval)
        {
            pCurrent->data = newval;
            break;
        }
        pCurrent = pCurrent->next;
    }
}

// 删除单链表的重复节点
void deleteDuplication(struct ListNode *head)
{
    if(head == NULL)
        return;
    
    struct ListNode *pCurrent = head;
    while(pCurrent != NULL)
    {
        struct ListNode *pNext = pCurrent;
        while(pNext->next)
        {
            if(pNext->next->data == pCurrent->data)
            {
                pNext->next = pNext->next->next;
            }
            else
            {
                pNext = pNext->next;
            }
        }
        pCurrent = pCurrent->next;
    }
}

// 输出该链表倒数第k个节点

struct ListNode *findKthToTail(struct ListNode *head, int k)
{
    if(head == NULL)
    {
        return NULL;
    }

    struct ListNode *fast = head;
    struct ListNode *last = head;

    for(int i = 0; i < k; i++)
    {
        fast = fast->next;
    }

    while(fast != NULL)
    {
        fast = fast->next;
        last = last->next;
    }
    return last;
}

int main(void)
{
    struct ListNode *head = initList();
    printList(head);
    insertNode(head, 2, 100);
    printList(head);
    deleteNode(head, 100);
    printList(head);
    changeNode(head, 2, 200);
    printList(head);
    deleteDuplication(head);
    printList(head);

}




