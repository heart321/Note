#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct ListNode 
{
	int val;
	struct ListNode* next;
}ListNode;

//初始化链表
 ListNode* list_init(void)
{
	ListNode* head = (ListNode*)malloc(sizeof(ListNode));
	head->val = -1;
	head->next = NULL;
	
	ListNode* pReal = head;
	
	int newValue = -1; 
	printf("init list(-1)end: ");
	while(1)
	{
		//1.输入数据
		
		scanf("%d",&newValue);
		if(newValue < 0) break; //
		
		//2.创建新的节点
		ListNode* newNode = (ListNode*)malloc(sizeof(ListNode));
		newNode->val = newValue;
		newNode->next = NULL;
		
		//3.将新节点插入head之后
		pReal->next = newNode;

		pReal = newNode;
	}
	
	return head;
}

//遍历链表
void printList(ListNode* head)
{
	if(head == NULL) return;
	
	//1.创建遍历链表的指针
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
	printList(head);
	
	return 0;
}