#ifndef HEADER_H
#define HEADER_H

#include <stddef.h>
#include <stdio.h> 
#define MAXTAB 10

typedef struct rec
{
    int Key; 
    int blkAddr;
    int recAddr;
} t_rec;

typedef struct blk
{
    t_rec Tab[MAXTAB];
    int Nrec;
}t_block;


typedef struct file_header
{
    int Nblocks;
} t_header;

typedef struct file
{
    FILE *f; //file pointer
    t_header h; //header in Main memmory
}t_TOF;

typedef struct T1 t_T1;
typedef struct T2 t_T2;

struct T1
{
    int V1;
    int V2;
    t_T1 *LC;
    t_T1 *RC;
    t_T2 *R; //root of T2 tree
};

struct T2
{
    int Key;
    int BlkNum;
    int RecNum;
    t_T2 *LC;
    t_T2 *RC;
};


typedef struct NodeStack
{
    t_T2 *Node;
    struct NodeStack *Next;
}t_Stack;


void TOF_open( t_TOF **F, char *fname, char mode );
void TOF_close( t_TOF *F );
void TOF_readBlock( t_TOF *F, long i, t_block *buf );
void TOF_writeBlock( t_TOF *F, long i, t_block *buf );
void setHeader( t_TOF *F , char *hname , long val );
long getHeader( t_TOF *F , char *hname );
void createTree(t_T1 **Root);
void freeTree(t_T1 **Root);
void LoadTreeFromFile(t_TOF *f, t_T1 **Root);
void insertInTree(t_T1 **Root, int Key, int BlkNum, int RecNum);
void insertInT2(t_T2 **Root, int Key, int BlkNum, int RecNum);
void allocateNodeT1(t_T1 **Node);
void allocateNodeT2(t_T2 **Node);
void SeveTreeToFile(t_TOF *f, t_T1 *Root);
void processT1(t_TOF *f, t_T1 *Node, int *Nblocks);
void processT2(t_TOF *f, t_T2 *Root, int *Nblocks);
void createStack(t_Stack **S);
void pushStack(t_Stack **S, t_T2 *Node);
void popStack(t_Stack **S, t_T2 **Node);
void searchKey(t_T1 *Root, int Key, int *Found, int *BlkNum, int *RecNum);



//B tree definitions and functions

typedef struct NodeBtree{
    int Key[4];// max 4 keys
    struct NodeBtree *child[5]; // max 5 children
    struct NodeBtree *parent; // pointer to parent node
    int degree; // number of keys + 1
}B_Tree;



int checkIfLeafNode(B_Tree *Node);
void allocateBTreeNode(B_Tree **Node);
void splitLeafNode(B_Tree **Root, B_Tree *fullLeafNode , int newKey);
void createLeafArray(B_Tree *LeafNode, int newKey, int (*tmpArr)[5]);
void createInternalArray(B_Tree *InternalNode, int middlevalue, B_Tree *newNode, int (*tmpArr)[5], B_Tree *(*tmpChildren)[6]);




#endif /* HEADER_H */
