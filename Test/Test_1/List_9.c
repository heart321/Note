#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ListNode
{
	int val;
	struct ListNode* next;
}ListNode;


/*初始化链表*/
ListNode* list_init(void)
{
	// 1.申请链表空间
	ListNode* head = (ListNode*)malloc(sizeof(ListNode));
	head->val = 0;
	head->next = NULL;
	
	// 2.创建遍历链表的指针
	ListNode* pCurrent = head;
	
	// 3.插入链表的数据 -1 结束
	printf("输入插入链表的数字(-1)结束：");
	int data = 0;
	while(1)
	{
		scanf("%d",&data);
		if(data == -1) break;
		
		// 4.创建要插入的节点
		ListNode* newNode = (ListNode*)malloc(sizeof(ListNode));
		newNode->val = data;
		newNode->next = NULL;
		
		// 5.将新节点插入链表
		pCurrent->next = newNode;
		pCurrent = pCurrent->next;
	}
	
	return head;
	
}

/* 在目标值节点后插入新节点 */
void list_insert(ListNode* head, int olddata, int newdata) {
    if (!head) return;

    ListNode *pCurrent = head->next;
    while (pCurrent) {
        if (pCurrent->val == olddata) {
            ListNode* newNode = malloc(sizeof(ListNode));
            newNode->val = newdata;
            // 将新节点插入到目标节点之后
            newNode->next = pCurrent->next;
            pCurrent->next = newNode;
            return; // 找到后立即返回
        }
        pCurrent = pCurrent->next;
    }

    // 未找到则尾插
    ListNode* tail = head;
    while (tail->next) tail = tail->next; // 定位到尾节点
    ListNode* newNode = malloc(sizeof(ListNode));
    newNode->val = newdata;
    newNode->next = NULL;
    tail->next = newNode;
}


/*删除节点*/
void list_delete(ListNode* head,int data)
{
	if(head == NULL) return;
	
	ListNode* pCurrent = head->next;
	ListNode* pRev = head;
	
	while(pCurrent != NULL)
	{
		if(pCurrent->val == data)
		{
			pRev->next = pCurrent->next;
			free(pCurrent);
			break;
		}
		pRev = pCurrent;
		pCurrent = pCurrent->next;
	}
}

/*寻找节点*/
ListNode* list_search(ListNode* head,int data)
{
	if(head == NULL) return NULL;
	
	ListNode* pCurrent = head->next;
	
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


/* 修改所有匹配的节点值 */
void list_change(ListNode* head, int olddata, int newdata) {
    if (!head) return;

    int found = 0;
    ListNode *pCurrent = head->next;
    while (pCurrent) {
        if (pCurrent->val == olddata) {
            pCurrent->val = newdata;
            found = 1;
        }
        pCurrent = pCurrent->next;
    }
    if (!found) printf("未找到值为 %d 的节点\n", olddata);
}



/*打印链表*/
void list_printf(ListNode* head)
{
	if(head == NULL) return;
	
	ListNode* pCurrent = head->next;
	
	while(pCurrent != NULL)
	{
		printf("%d ",pCurrent->val);
		pCurrent = pCurrent->next;
	}
	printf("\n");
}

int main()
{
	ListNode* head = list_init();
	list_printf(head);
	
	list_insert(head,10,100);
	list_printf(head);
	
	list_delete(head,20);
	list_printf(head);
	
	list_chage(head,30,40);
	list_printf(head);
	
	return 0;
}
