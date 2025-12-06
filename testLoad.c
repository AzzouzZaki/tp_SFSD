#include <stdio.h>
#include <stdlib.h>
#include "header.h"

// helper function to print T2 tree (inorder)
void PrintT2(t_T2 *root) {
    if (root == NULL) return;
    PrintT2(root->LC);
    printf("   [Key=%d | Block=%d | Rec=%d]\n", root->Key, root->BlkNum, root->RecNum);
    PrintT2(root->RC);
}

// helper function to print T1 tree (inorder)
void PrintT1(t_T1 *root) {
    if (root == NULL) return;
    PrintT1(root->LC);
    printf("\nT1 Node: Range [%d , %d]\n", root->V1, root->V2);
    PrintT2(root->R);
    PrintT1(root->RC);
}

int main(void) {

    t_TOF *F = NULL;
    t_T1 *Root = NULL;

    printf("Creating TOF file...\n");

    TOF_open(&F, "test.tof", 'N');

    // ---- create test blocks ----

    t_block B;

    // Block 1
    B.Nrec = 3;
    B.Tab[0].Key = 10;
    B.Tab[0].blkAddr = 1;
    B.Tab[0].recAddr = 1;
    B.Tab[1].Key = 20;
    B.Tab[1].blkAddr = 2;
    B.Tab[1].recAddr = 2;
    B.Tab[2].Key = 30;
    B.Tab[2].blkAddr = 1;
    B.Tab[2].recAddr = 3;
    B.Nrec = 3;
    TOF_writeBlock(F, 1, &B);

    // Block 2
    B.Nrec = 3;
    B.Tab[0].Key = 40;
    B.Tab[0].blkAddr = 2;
    B.Tab[0].recAddr = 5;
    B.Tab[1].Key = 50;
    B.Tab[1].blkAddr = 2;
    B.Tab[1].recAddr = 6;
    B.Tab[2].Key = 60;
    B.Tab[2].blkAddr = 2;
    B.Tab[2].recAddr = 7;
    B.Nrec = 3;
    
    TOF_writeBlock(F, 2, &B);

    setHeader(F, "Nblocks", 2);

    TOF_close(F);   // save header and close


    // ---- OPEN FILE IN READ MODE ----

    printf("Reopening file...\n");

    TOF_open(&F, "test.tof", 'E');


    // ---- LOAD TREE FROM FILE ----

    printf("Loading index tree...\n");

    LoadTreeFromFile(F, &Root);


    // ---- DISPLAY TREE ----

    printf("\n===== INDEX TREE =====\n");
    PrintT1(Root);


    // ---- CLEANUP ----

    freeTree(&Root);
    TOF_close(F);

    printf("\nTest complete.\n");

    return 0;
}

