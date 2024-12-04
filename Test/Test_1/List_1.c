#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct listnode
{
    int data;
    struct listnode *next;
};

struct listnode* Init_List(void)
{
    struct listnode *head = (struct listnode *)malloc(sizeof(struct listnode));
    head->data = -1;
    head->next = NULL;

    struct listnode *pReal = head;

    int val = -1;
    printf("List Initialized:\n");
    while (1)
    {
        
        scanf("%d", &val);

        if (val == -1)
        {
            break;
        }

        struct listnode *pNew = (struct listnode *)malloc(sizeof(struct listnode));
        pNew->data = val;
        pNew->next = NULL;

        //insert
        pReal->next = pNew;

        //set current
        pReal = pNew;
    }
    return head;
}


//insert val to the list
void Insert_List(struct listnode *head, int oldval,int newval)
{
    if(head == NULL)
    {
        return;
    }
    //create a two pointer
    struct listnode *pCurrent = head->next;
    struct listnode *pPrev = head;

    while (pCurrent != NULL)
    {
        if (pCurrent->data == oldval)
        {
            struct listnode *pNew = (struct listnode *)malloc(sizeof(struct listnode));
            pNew->data = newval;
            pNew->next = NULL;

            //insert
            pPrev->next = pNew;
            pNew->next = pCurrent;

            break;
        }
        pPrev = pCurrent;
        pCurrent = pCurrent->next;
        
    }
    
    //if not found
    if (pCurrent == NULL)
    {
        struct listnode *pNew = (struct listnode *)malloc(sizeof(struct listnode));
        pNew->data = newval;
        pNew->next = NULL;

        //insert
        pPrev->next = pNew;       
    }
}

//delete val from the list
void Delete_List(struct listnode *head, int val)
{
    if(head == NULL)
    {
        return;
    }
    struct listnode *pCurrent = head->next;
    struct listnode *pPrev = head;

    while (pCurrent != NULL)
    {
        if (pCurrent->data == val)
        {
            //delete
            pPrev->next = pCurrent->next;

            //free  memory
            free(pCurrent);
            pCurrent = NULL;

            break;
        }
        pPrev = pCurrent;
        pCurrent = pCurrent->next;
    }
}

//clear the list
void Clear_List(struct listnode *head)
{
    if (head == NULL)
    {
        /* code */
        return;
    }
    
    struct listnode *pCurrent = head;
    while (pCurrent != NULL)
    {
        struct listnode *pNext = pCurrent->next;
        free(pCurrent);
        pCurrent = NULL;

        pCurrent = pNext;
    }
}

//removeDuplicate node from the list
void RemoveDuplicate_List(struct listnode *head)
{
    if (head == NULL)
    {
        return;
    }
    //two pointer
    struct listnode *pCurrent = head;
    while(pCurrent)
    {
        struct listnode *pPrev = pCurrent;
        while (pPrev->next)
        {
            if(pPrev->next->data == pCurrent->data)
            {
                pPrev->next = pPrev->next->next;
            }
            else
            {
                pPrev = pPrev->next;
            }
        }
        pCurrent = pCurrent->next;
    }
}


void Foreach_List(struct listnode *head)
{
    if (head == NULL)
    {
        return;
    }
    struct listnode *pCurrent = head->next;
    while (pCurrent != NULL)
    {
        printf("%d\n", pCurrent->data);
        pCurrent = pCurrent->next;
    }
    printf("\n");
}


int main(int argc, char const *argv[])
{
    struct listnode *head = Init_List();

    //RemoveDuplicate_List(head);

    Foreach_List(head);

    return 0;
}
