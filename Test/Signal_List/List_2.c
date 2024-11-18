#include <stdio.h>
#include <stdlib.h>

struct listnode
{
    int data;
    struct listnode *next;
};

// initializing the list
struct listnode *Init_list(void)
{
    struct listnode *head = (struct listnode *)malloc(sizeof(struct listnode));
    head->data = 0;
    head->next = NULL;

    struct listnode *pCurrent = head;

    int val = -1;
    printf("Enter the value to be added to the list: ");
    while (1)
    {

        scanf("%d", &val);

        if (val == -1)
        {
            break;
        }

        struct listnode *newNode = (struct listnode *)malloc(sizeof(struct listnode));
        newNode->data = val;
        newNode->next = NULL;

        pCurrent->next = newNode;

        pCurrent = newNode;

    } // end while
    return head;
}

// insertion at the end of the list
void Insert_list(struct listnode *head, int oldval, int newval)
{
    if (head == NULL)
    {
        return;
    }

    // two pointers
    struct listnode *pCurrent = head->next;
    struct listnode *pPrev = head;

    while (pCurrent != NULL)
    {
        if (pCurrent->data == oldval)
        {
            struct listnode *newNode = (struct listnode *)malloc(sizeof(struct listnode));
            newNode->data = newval;
            newNode->next = NULL;

            pPrev->next = newNode;
            newNode->next = pCurrent;

            break;
        }
        pPrev = pCurrent;
        pCurrent = pCurrent->next;
    }

    // insert at the end
    if (pCurrent == NULL)
    {
        struct listnode *newNode = (struct listnode *)malloc(sizeof(struct listnode));
        newNode->data = newval;
        newNode->next = NULL;

        pPrev->next = newNode;
    }
}

// deletion from the list
void Delete_list(struct listnode *head, int val)
{
    if (head == NULL)
    {
        return;
    }
    struct listnode *pCurrent = head->next;
    struct listnode *pPrev = head;

    while (pCurrent != NULL)
    {
        if (pCurrent->data == val)
        {
            pPrev->next = pCurrent->next;
            free(pCurrent);
            break;
        }
        pPrev = pCurrent;
        pCurrent = pCurrent->next;
    }
}

// searching in the list
struct listnode* Search_list(struct listnode *head, int val)
{
    if (head == NULL)
    {
        return -1;
    }
    struct listnode *pCurrent = head->next;

    while (pCurrent != NULL)
    {
        if (pCurrent->data == val)
        {
            return pCurrent;
        }
        pCurrent = pCurrent->next;
    }
    return -1;
}

//chage the value of the node
void Change_list(struct listnode *head, int oldval, int newval)
{
    if (head == NULL)
    {
        return;
    }
    struct listnode *pCurrent = head->next;

    while (pCurrent != NULL)
    {
        if (pCurrent->data == oldval)
        {
            pCurrent->data = newval;        

            break;  

        }
        pCurrent = pCurrent->next;
    }

}

//remove duplicates
void Remove_duplicates(struct listnode *head)
{
    if(head == NULL)
    {
        return;
    }
    struct listnode *pCurrent = head;
    while (pCurrent != NULL)
    {
        struct listnode *pPrev = pCurrent;
        while(pPrev->next != NULL)
        {
            if(pPrev->next->data == pCurrent->data)
            {
                pPrev->next = pPrev->next->next;
            }
            else
            {
                pPrev = pPrev->next;
            }
        }//end while
        pCurrent = pCurrent->next;
    }
}

// for printing the list
void Print_list(struct listnode *head)
{
    struct listnode *pCurrent = head->next;

    printf("The list is: ");
    while (pCurrent != NULL)
    {
        printf("%d ", pCurrent->data);
        pCurrent = pCurrent->next;
    }
    printf("\n");
}

int main()
{
    struct listnode *head = Init_list();
    Insert_list(head, 2, 10);
    Print_list(head);
}