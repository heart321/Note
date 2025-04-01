//
// Created by 15819 on 2025/3/17.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct ListNode
{
    int value;
    struct ListNode* next;
};

struct ListNode* List_Init()
{
    struct ListNode* head = (struct ListNode*)malloc(sizeof(struct ListNode));
    if(NULL == head)
    {
        return NULL;
    }
    head->value = 0;
    head->next = NULL;

    struct ListNode* pCurrent = head;
    printf("input value(-1 end):");

    int data = 0;
    while (1)
    {
        scanf("%d", &data);
        if(data < 0) break;
        struct ListNode* pNewNode = malloc(sizeof (struct ListNode));
        pNewNode->value = data;
        pNewNode->next = NULL;

        pCurrent->next = pNewNode;
        pCurrent = pCurrent->next;

    }
    return head;
}

void List_Insert(struct ListNode* head,int oldData,int newData)
{
    if(head == NULL)
    {
        return;
    }

    struct ListNode* pCurrent = head->next;
    struct ListNode* pRev = head;

    while (pCurrent != NULL)
    {
        if(pCurrent->value == oldData)
        {
            struct ListNode* newNode = (struct ListNode*)malloc(sizeof(struct ListNode));
            newNode->value = newData;
            newNode->next = NULL;

            pRev->next = newNode;
            newNode->next = pCurrent;
            break;
        }

        pRev = pCurrent;
        pCurrent = pCurrent->next;
    }

    if(pCurrent == NULL)
    {
        struct ListNode* newNode = (struct ListNode*) malloc(sizeof(struct ListNode));
        newNode->value = newData;
        newNode->next = NULL;

        pRev->next = newNode;
    }

}
// 修改ListNode
void List_Change(struct ListNode* head,int oldData,int newData)
{
    if(head == NULL)
    {
        return;
    }

    struct ListNode* pCurrent = head;
    while (pCurrent !=NULL)
    {
        if(pCurrent->value == oldData)
        {
            pCurrent->value = newData;
        }
        pCurrent = pCurrent->next;
    }
}

void List_Print(struct ListNode* head)
{
    if(NULL == head)
    {
        return;
    }

    struct ListNode* pCurrent = head->next;
    while (NULL !=  pCurrent)
    {
        printf("%d ",pCurrent->value);
        pCurrent = pCurrent->next;
    }
    printf("\n");

}


int main()
{
    struct ListNode* head = List_Init();
    List_Print(head);
    List_Insert(head,80,10);
    List_Print(head);
    List_Change(head,60,100);
    List_Print(head);


    return 0;
}