#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct BiTNode {
    int data;
    struct BiTNode *left; // left child
    struct BiTNode *right;  // right child
} BiTNode, *BiTree;

// 初始化二叉树
void InitBiTree(BiTree *T) {
    *T = (BiTree)malloc(sizeof(BiTNode));
    //根节点
    (*T)->data = 1;
    (*T)->left = (BiTree)malloc(sizeof(BiTNode));
    (*T)->right = (BiTree)malloc(sizeof(BiTNode));

    //1节点的左孩子2
    (*T)->left->data = 2;
    (*T)->left->left = (BiTree)malloc(sizeof(BiTNode));
    (*T)->left->right = (BiTree)malloc(sizeof(BiTNode));

    //2节点的右孩子5
    (*T)->left->right->data = 5;
    (*T)->left->right->left = NULL;
    (*T)->left->right->right = NULL;

    //1节点的右孩子3
    (*T)->right->data = 3;
    (*T)->right->left = (BiTree)malloc(sizeof(BiTNode));

    //3节点的左孩子6
    (*T)->right->left->data = 6;
    (*T)->right->left->left = NULL;
    (*T)->right->left->right = NULL;
    (*T)->right->right = (BiTree)malloc(sizeof(BiTNode));

    //3节点的右孩子7
    (*T)->right->right->data = 7;
    (*T)->right->right->left = NULL;
    (*T)->right->right->right = NULL;

    //2节点的左孩子4
    (*T)->left->left->data = 4;
    (*T)->left->left->left = NULL;
    (*T)->left->left->right = NULL;

}

// 输出节点本身的数据
void visit(BiTree T) {
    printf("%d ", T->data);
}

//先序遍历
void PreOrderTraverse(BiTree T) {
    if(T){
        visit(T); //访问根节点
        PreOrderTraverse(T->left); //访问左子树
        PreOrderTraverse(T->right); //访问右子树
    }

    return; //递归结束条件
}

//中序遍历
void InOrderTraverse(BiTree T) {
    if(T){
        InOrderTraverse(T->left); //访问左子树
        visit(T); //访问根节点
        InOrderTraverse(T->right); //访问右子树
    }

    return; //递归结束条件
}

//后序遍历
void PostOrderTraverse(BiTree T) {
    if(T){
        PostOrderTraverse(T->left); //访问左子树
        PostOrderTraverse(T->right); //访问右子树
        visit(T); //访问根节点
    }

    return; //递归结束条件
}

// 求二叉树的深度
int Depth(BiTree T) {
    if(T == NULL)
    {
        return 0;
    }
    
    int leftLength = Depth(T->left);
    int rightLength = Depth(T->right);

    return leftLength > rightLength ? leftLength + 1 : rightLength + 1;
}

// 判断二叉树是否相等
void PreOrder(struct BiTNode *root, int *arr,int *index) {
    (*index)++;
    if(root == NULL) {
        arr[*index] = 0;
        return;
    }
    arr[*index] = root->data;
    PreOrder(root->left, arr, index);
    PreOrder(root->right, arr, index);
}
bool isEqual(BiTree T1, BiTree T2) {
    int i = 0,j = 0;
    int arr1[1000], arr2[1000];
    memset(arr1, 0, sizeof(arr1));
    memset(arr2, 0, sizeof(arr2));
    if(T1 == NULL && T2 == NULL) {
        return true;
    }
    if(T1 == NULL || T2 == NULL) {
        return false;
    }
    PreOrder(T1, arr1, &i);
    PreOrder(T2, arr2, &j);
    for(int k = 0;k < 1000;k++) {
        if(arr1[k] != arr2[k]) {
            return false;
        }
    }
    return true;

}

// 判断二叉树是不是平衡二叉树
bool isBalanced(BiTree T) {
    if(T == NULL) {
        return true;
    }
    int leftDepth = Depth(T->left);
    int rightDepth = Depth(T->right);

    if(abs(leftDepth - rightDepth) < 2 == false) {
        return false;
    }

    return isBalanced(T->left) && isBalanced(T->right) && abs(leftDepth - rightDepth) < 2;
}

int main(int argc, char const *argv[])
{
    BiTree T;
    InitBiTree(&T);
    printf("先序遍历结果为：\n");
    PreOrderTraverse(T);
    printf("\n中序遍历结果为：\n");
    InOrderTraverse(T);
    printf("\n后序遍历结果为：\n");
    PostOrderTraverse(T);
    return 0;
}


