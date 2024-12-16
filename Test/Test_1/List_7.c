#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct ListNode{
    int val;
    struct ListNode *next;
};

//初始化链表
struct ListNode* List_Init(void)
{

  struct ListNode* head = (struct ListNode*)malloc(sizeof (struct ListNode));
  head->val = -1;
  head->next = NULL;

  struct ListNode* pReal = head;

  int data = -1;
  printf("Init List(-1 end):\n");
  while (1)
  {
    scanf("%d",&data);
    if(data < 0) break;

    struct ListNode* newNode = (struct ListNode*) malloc(sizeof (struct ListNode));
    newNode->val = data;
    newNode->next = NULL;

    pReal->next = newNode;

    pReal = newNode;
  }
  return  head;

}

//打印链表
void Print_List(struct ListNode* head)
{
  if(head == NULL)
  {
    return;
  }

  struct ListNode* pCurrent = head->next;

  while(pCurrent != NULL)
  {
    printf("%d ",pCurrent->val);
    pCurrent = pCurrent->next;
  }
  printf("\n");
}

// 插入节点
void Insert_List(struct ListNode* head,int oldVal,int newVal)
{
    if(head == NULL) return;

    struct ListNode* pCurrent = head->next;
    struct ListNode* pPrev = head;

    while(pCurrent != NULL)
    {
        if(pCurrent->val == oldVal)
        {
            struct ListNode* newNode = (struct ListNode*)malloc(sizeof (struct ListNode));
            newNode->val = newVal;
            newNode->next = NULL;

            //insert
            pPrev->next = newNode;
            newNode->next = pCurrent;

            break;
        }
        pPrev = pCurrent;
        pCurrent = pCurrent->next;
    }

    if(pCurrent == NULL)
    {
        struct ListNode* newNode = (struct ListNode*) malloc(sizeof (struct ListNode));
        newNode->val = newVal;
        newNode->next = NULL;

        pPrev->next = newNode;
    }
}

// 删除链表中的节点
void Delete_ListNoed(struct ListNode* head,int val)
{
    if(head == NULL) return;
    struct ListNode* pCurrent = head->next;
    struct ListNode* pPrev = head;

    while(pCurrent != NULL)
    {
        if(pCurrent->val == val)
        {
            pPrev->next = pCurrent->next;
            free(pCurrent);
            pCurrent = NULL;
            break;
        }
        pCurrent = pPrev;
        pCurrent = pCurrent->next;
    }
}

// 清空链表
void Clear_List(struct ListNode* head)
{
    if(head == NULL) return;

    struct ListNode* pCurrent = head;
    while(pCurrent != NULL)
    {
        struct ListNode* pNext = pCurrent->next;
        free(pCurrent);
        pCurrent=NULL;

        pCurrent = pNext;
    }

}

// 删除链表中的重复值
struct ListNode* deleteDuplicate(struct ListNode* head) {
    if (head == NULL) return NULL;

    struct ListNode *pCurrent = head;
    while (pCurrent != NULL)
    {
        struct ListNode* pNext = pCurrent;
        while(pNext->next != NULL)
        {
            if(pNext->next->val == pCurrent->val)
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
    return head;
}

// 找出链表中倒数第k个节点
struct ListNode* findKthToTail(struct ListNode* head,int k)
{
    if(head == NULL) return NULL;

    struct ListNode* fast = head;
    struct ListNode* slow = head;

    for(int i = 0;i < k;i++)
    {
        if(fast == NULL)
        {
            return NULL;
        }
        fast = fast->next;
    }

    while (fast)
    {
        fast = fast->next;
        slow = slow->next;
    }

    return  slow;

}

// 找出链表的中间节点
struct  ListNode* findMidNode(struct ListNode* head)
{
    if(head == NULL) return NULL;

    struct ListNode* fast = head;
    struct ListNode* slow = head;

    while (fast && fast->next)
    {
        slow = slow->next;
        fast = fast->next->next;
    }

    return  slow;

}


// 翻转链表
struct ListNode* reverseList(struct ListNode* head)
{
    if(head == NULL || head->next) return head;

    struct ListNode* mid = head;
    struct ListNode* later = head->next;
    head = reverseList(later);
    later->next = mid;
    mid->next = NULL;

    return head;
}

// 环形链表
struct ListNode* detectCycle(struct ListNode* head)
{
    if(head == NULL) return NULL;

    struct ListNode* fast = head;
    struct ListNode* slow = head;

    while (fast && fast->next)
    {
        fast = fast->next->next;
        slow = slow->next;
        if(fast == slow)
        {
            fast = hed;
            while (fast != head)
            {
                fast = fast->next;
                slow = slow->next;
            }
            return fast;
        }

    }
    return  NULL;
}

// 回文链表
bool isPalindrome(struct ListNode* head)
{
    struct ListNode* fast = head;
    struct ListNode* slow = head;

    while (fast && fast->next)
    {
        fast = fast->next->next;
        slow = slow->next;
    }

    struct ListNode* cur = slow;
    struct ListNode* pre = NULL;

    while (pre)
    {
        struct ListNode* pTemp = cur->next;

        cur->next = pre;
        pre = cur;
        cur = pTemp;
    }

    while(pre && head)
    {
        if(pre->val != head->val)
        {
            return false;
        }
        pre=pre->next;
        head=head->next;
    }
    return true;
}



int main(int argc, char const *argv[])
{
    struct ListNode* head = List_Init();
    Print_List(head);
    return 0;
}
