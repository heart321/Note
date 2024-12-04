#include<stdio.h>
#include<stdlib.h>

//create struct listnode
typedef struct {
    int val;
    struct listnode *next;
} listnode;

// initialize the listnode
listnode *Init_list(void)
{
    listnode *head = (listnode *)malloc(sizeof(listnode));
    head->val = -1;
    head->next = NULL;
    
    listnode *pCurrent = head;

    int data = -1;
    printf("Enter the data (-1 to stop): ");
    while (data != -1)
    {
        scanf("%d", &data);
        if (data != -1)
        {
            listnode *pNew = (listnode *)malloc(sizeof(listnode));
            pNew->val = data;
            pNew->next = NULL;

            pCurrent->next = pNew;
            pCurrent = pNew;
        }
    } //end while

    return head;
}

//insert node
void Insert_list(listnode *head, int olddata,int newdata)
{
    if(head == NULL)
    {
        printf("The list is empty!\n");
        return;
    }
    
    //two pointers
    listnode *pCurrent = head->next;
    listnode *pPrev = head;

    while(pCurrent != NULL)
    {
        if(pCurrent->val == olddata)
        {
            listnode *pNew = (listnode *)malloc(sizeof(listnode));
            pNew->val = newdata;
            pNew->next = pCurrent->next;

            pPrev->next = pNew;
            pNew->next = pCurrent;
            break;
        }
        pPrev = pCurrent;
        pCurrent = pCurrent->next;
    }

    //insert at the end
    if(pCurrent == NULL)
    {
        listnode *pNew = (listnode *)malloc(sizeof(listnode));
        pNew->val = newdata;
        pNew->next = NULL;

        pPrev->next = pNew;
    }
}

//delete node
void Delete_list(listnode *head, int data)
{
    if(head == NULL)
    {
        printf("The list is empty!\n");
        return;
    }

    //two pointers
    listnode *pCurrent = head->next;
    listnode *pPrev = head;

    while(pCurrent != NULL)
    {
        if(pCurrent->val == data)
        {
            pPrev->next = pCurrent->next;
            free(pCurrent); //free the memory
            break;
        }
        pPrev = pCurrent;
        pCurrent = pCurrent->next;
    }
}


//search node
listnode *Search_list(listnode *head, int data)
{
    if(head == NULL)
    {
        printf("The list is empty!\n");
        return NULL;
    }

    listnode *pCurrent = head->next;

    while(pCurrent != NULL)
    {
        if(pCurrent->val == data)
        {
            return pCurrent;
        }
        pCurrent = pCurrent->next;
    }

    return NULL;
}

//chage node
void Change_list(listnode *head, int olddata, int newdata)
{
    if(head == NULL)
    {
        printf("The list is empty!\n");
        return;
    }

    listnode *pCurrent = head->next;

    while(pCurrent != NULL)
    {
        if(pCurrent->val == olddata)
        {
            pCurrent->val = newdata;
            break;
        }
        pCurrent = pCurrent->next;
    }
}

//print node
void Print_list(listnode *head)
{
    if(head == NULL)
    {
        printf("The list is empty!\n");
        return;
    }

    listnode *pCurrent = head->next;

    while(pCurrent != NULL)
    {
        printf("%d ", pCurrent->val);
        pCurrent = pCurrent->next;
    }
    printf("\n");
}
int main(int argc, char const *argv[])
{
    listnode *head = Init_list();
    Insert_list(head, 2, 3);
    Print_list(head);
    
    return 0;
}
