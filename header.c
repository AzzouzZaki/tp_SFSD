#include "header.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>



void TOF_open( t_TOF **F, char *fname, char mode )
{
   *F = malloc( sizeof( t_TOF) );
   if ( mode == 'E' || mode == 'e' ) { 
        // openning an existing TOF file ...
	(*F)->f = fopen( fname, "rb+" );
	if ( (*F)->f == NULL ) { 
           perror( "TOF_open" );
           exit( EXIT_FAILURE );
        }
        // loading header part in main memory (in (*F)->h)
	fread( &((*F)->h), sizeof(t_header), 1, (*F)->f );
   }
   else { // mode == 'N' || mode == 'n'
        // creating a new TOF file ... 
	(*F)->f = fopen( fname, "wb+" );
	if ( (*F)->f == NULL ) {
           perror( "TOF_open" );
           exit( EXIT_FAILURE );
        }
        // initializing the header part in main memory (in (*)->h)
	(*F)->h.Nblocks = 0;
        // saving the header part on disk
	fwrite( &((*F)->h), sizeof(t_header), 1, (*F)->f );
   }
}


void TOF_close( t_TOF *F )
{
   // saving header part in secondary memory (at the begining of the stream F->f)
   fseek(F->f, 0L, SEEK_SET);
   fwrite( &F->h, sizeof(t_header), 1, F->f );
   fclose(F->f);
   free( F );
}


// reading data block number i into variable buf
void TOF_readBlock( t_TOF *F, long i, t_block *buf )
{
   fseek(F->f, sizeof(t_header) + (i-1)*sizeof(t_block), SEEK_SET);
   fread( buf, sizeof(t_block), 1, F->f );
}


// writing the contents of the variable buf in data block number i
void TOF_writeBlock( t_TOF *F, long i, t_block *buf )
{
   fseek(F->f, sizeof(t_header) + (i-1)*sizeof(t_block), SEEK_SET);
   fwrite( buf, sizeof(t_block), 1, F->f );
}

// header modification
void setHeader( t_TOF *F , char *hname , long val )
{
   if ( strcmp( hname , "Nblocks" ) == 0 ) { F->h.Nblocks = val; return; }
   fprintf(stderr, "setHeader : Unknown headerName: \"%s\"\n", hname);
}


long getHeader( t_TOF *F , char * hname )
{
   if ( strcmp( hname , "Nblocks" ) == 0 ) return F->h.Nblocks;
   fprintf(stderr, "getHeader : Unknown headerName: \"%s\"\n", hname);
   return 0;
}

void createTree(t_T1 **Root){
    *Root = NULL;
}

void freeTree(t_T1 **Root){
    if(*Root != NULL){
        freeTree(&((*Root)->LC));
        freeTree(&((*Root)->RC));
        free(*Root);
        *Root = NULL;
    }
}

void allocateNodeT1(t_T1 **Node){
    *Node = malloc(sizeof(t_T1));
    (*Node)->R = NULL;
    (*Node)->LC = NULL;
    (*Node)->RC = NULL;
}

void allocateNodeT2(t_T2 **Node){
    *Node = malloc(sizeof(t_T2));
    (*Node)->LC = NULL;
    (*Node)->RC = NULL;
    
}


void createStack(t_Stack **S){
    *S = NULL;
}

void pushStack(t_Stack **S, t_T2 *Node){
    t_Stack *newNode = malloc(sizeof(t_Stack));
    if (!newNode) return; // allocation failed
    newNode->Node = Node; /* store pointer, not copy */
    newNode->Next = *S;
    *S = newNode;
}

void popStack(t_Stack **S, t_T2 **Node){
    if(*S == NULL) return; // empty
    t_Stack *temp = *S;
    *Node = temp->Node; /* return pointer to the caller */
    *S = temp->Next; // update head
    free(temp);
}


void LoadTreeFromFile(t_TOF *f, t_T1 **Root){ //we assume the file was opened in read mode
    createTree(Root);
    long nBlocks = getHeader(f, "Nblocks");
    int i = 0;
    t_block Buf;
    while(i < nBlocks){
        TOF_readBlock(f, i+1, &Buf);
        for(int j = 0; j < Buf.Nrec; j++){
            insertInTree(Root, Buf.Tab[j].Key, Buf.Tab[j].blkAddr, Buf.Tab[j].recAddr);
        }
        i++;
    }
}


void insertInTree(t_T1 **Root, int Key, int BlkNum, int RecNum){
    if(*Root == NULL){
        allocateNodeT1(Root);
        (*Root)->V1 = Key;
        (*Root)->V2 = Key;
        insertInT2(&((*Root)->R), Key, BlkNum, RecNum);
        return;
    }
    t_T1 *current = *Root;
    while (current != NULL)
    {
        if(current->V1 == current->V2 && Key < current->V1){
            current->V1 = Key;
            insertInT2(&(current->R), Key, BlkNum, RecNum);
            return;
        }
        if(current->V1 == current->V2 && Key > current->V2){
            current->V2 = Key;
            insertInT2(&(current->R), Key, BlkNum, RecNum);
            return;
        }
        if(Key < current->V1){
            if(current->LC != NULL){
                current = current->LC;
            }else{
                t_T1 *newNode;
                allocateNodeT1(&newNode);
                newNode->V1 = Key;
                newNode->V2 = Key;
                current->LC = newNode;
                insertInT2(&(newNode->R), Key, BlkNum, RecNum);
                return;
            }
            }else
            {
                if(Key > current->V2){
                    if(current->RC != NULL){
                        current = current->RC;
                    }else{
                        t_T1 *newNode;
                        allocateNodeT1(&newNode);
                        newNode->V1 = Key;
                        newNode->V2 = Key;
                        current->RC = newNode;
                        insertInT2(&(newNode->R), Key, BlkNum, RecNum);
                        return;
                    }
                }
                else{
                    insertInT2(&(current->R), Key, BlkNum, RecNum);
                    return;
            }
            
        }
    }
    
}


void insertInT2(t_T2 **Root, int Key, int BlkNum, int RecNum){
    if(*Root == NULL){
        allocateNodeT2(Root);
        (*Root)->Key = Key;
        (*Root)->BlkNum = BlkNum;
        (*Root)->RecNum = RecNum;
        return;
    }
    t_T2 *current = *Root;
    while (current != NULL)
    {
        if(Key < current->Key){
            if(current->LC != NULL){
                current = current->LC;
            }else{
                t_T2 *newNode;
                allocateNodeT2(&newNode);
                newNode->Key = Key;
                newNode->BlkNum = BlkNum;
                newNode->RecNum = RecNum;
                current->LC = newNode;
                return;
            }
        }else{
            if(current->RC != NULL){
                current = current->RC;
            }else{
                t_T2 *newNode;
                allocateNodeT2(&newNode);
                newNode->Key = Key;
                newNode->BlkNum = BlkNum;
                newNode->RecNum = RecNum;
                current->RC = newNode;
                return;
            }
        }
    }
}


void SeveTreeToFile(t_TOF *f, t_T1 *Root){ //we assume the file was opened in write mode
    int Nblocks = 0;
    processT1(f, Root, &Nblocks);
    setHeader(f, "Nblocks", Nblocks);
    printf("Tree saved to output.tof\n");
}

void processT1(t_TOF *f, t_T1 *Node, int *Nblocks){
   if(Node != NULL){
        processT1(f, Node->LC, Nblocks);
        processT2(f, Node->R, Nblocks);
        processT1(f, Node->RC, Nblocks);
   }
}


void processT2(t_TOF *f, t_T2 *Root, int *Nblocks){
    if(Root != NULL){
        int finished = 0;
        t_T2 *current = Root;
        int j = 0;
        t_block Buf;
        Buf.Nrec = 0;
        t_Stack *S;
        createStack(&S);
        while(!finished){
            while (current != NULL)
            {
                pushStack(&S,current);
                current = current->LC;
            }
            if(S == NULL){
                finished = 1;
            }else{
                popStack(&S, &current);

                Buf.Tab[j].Key = current->Key;
                Buf.Tab[j].blkAddr = current->BlkNum;
                Buf.Tab[j].recAddr = current->RecNum;
                Buf.Nrec = j + 1;
                j++;

                if(j > MAXTAB){
                    TOF_writeBlock(f, *Nblocks , &Buf);
                    (*Nblocks)++;
                    j = 0;
                }
               
                current = current->RC;
            }  
         
        }
         if (j > 0) {
            Buf.Nrec = j;
            TOF_writeBlock(f, *Nblocks + 1, &Buf);
            (*Nblocks)++;
        } 
    }
}




void searchKey(t_T1 *Root, int Key, int *Found, int *BlkNum, int *RecNum){
    *Found = 0;
    t_T1 *current = Root;
    while(current != NULL && *Found == 0){
        if(Key < current->V1){
            current = current->LC;
        }else
        {
            if(Key > current->V2){
                current = current->RC;
        }else
        {
            t_T2 *currentT2 = current->R;
            while(currentT2 != NULL && *Found == 0){
                if(Key < currentT2->Key){
                    currentT2 = currentT2->LC;
                }else
                {
                    if(Key > currentT2->Key){
                        currentT2 = currentT2->RC;
                    }else
                    {
                        *Found = 1;
                        *BlkNum = currentT2->BlkNum;
                        *RecNum = currentT2->RecNum;
                    }
                }
            }
        }
    }
};
}

// B TREE FUNCTIONS

int checkIfLeafNode(B_Tree *Node){
    if(Node == NULL) return 0;
    if(Node->child[0] == NULL && Node->child[1] == NULL && Node->child[2] == NULL && Node->child[3] == NULL && Node->child[4] == NULL ) return 1;
    return 0;
}


void allocateBTreeNode(B_Tree **Node){
    *Node = malloc(sizeof(B_Tree));
    for(int i = 0; i < 5; i++){
        (*Node)->child[i] = NULL;
    }
    (*Node)->parent = NULL;
    (*Node)->degree = 0;
    for(int i = 0; i < 4; i++){
        (*Node)->Key[i] = 0;
    }
}




void createLeafArray(B_Tree *LeafNode, int newKey, int (*tmpArr)[5]){
    if(LeafNode == NULL) return;
    
    int i = 0;
    int processed = 0;
    while (i < 5) //copy existing keys and insert new key in order
    {
        if(newKey < LeafNode->Key[i] && processed == 0){
            (*tmpArr)[i] = newKey;
            processed = 1;
        }else
        {
            if(processed == 1){
                (*tmpArr)[i] = LeafNode->Key[i - 1];
            }else
            {
                (*tmpArr)[i] = LeafNode->Key[i];
            }
        }
        i++;
    }
    if(processed == 0){ //new key is the largest
        (*tmpArr)[4] = newKey;
    }
}



void splitLeafNode(B_Tree **Root, B_Tree *fullLeafNode , int newKey, int *middleValue, B_Tree **newLeftNode, B_Tree **newRightNode){
    if(fullLeafNode == NULL || Root == NULL) return;
    if(!checkIfLeafNode(fullLeafNode)) {
        return;
    }
    int tmpArr[5];
    createLeafArray(fullLeafNode, newKey, &tmpArr);
    *middleValue = tmpArr[2]; //the middle value to be promoted
    //create left and right nodes
    allocateBTreeNode(newLeftNode);
    allocateBTreeNode(newRightNode);
    //populate left node
    for(int i = 0; i < 2; i++){
        (*newLeftNode)->Key[i] = tmpArr[i];
        (*newLeftNode)->degree = 3;
    }
    //populate right node
    for(int i = 3; i < 5; i++){
        (*newRightNode)->Key[i - 3] = tmpArr[i];
        (*newRightNode)->degree = 3;
    }
}