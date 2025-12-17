#include "header.h"
#include <stdlib.h>
#include <stdio.h>

/* ------------------ Display functions ------------------ */
void displayT2(t_T2 *Root)
{
    if (Root != NULL) {
        displayT2(Root->LC);
        printf("(%d | B:%d R:%d) ", Root->Key, Root->BlkNum, Root->RecNum);
        displayT2(Root->RC);
    }
}

void displayT1(t_T1 *Root)
{
    if (Root != NULL) {
        displayT1(Root->LC);
        printf("\nT1 Node [V1=%d , V2=%d] -> T2: ", Root->V1, Root->V2);
        displayT2(Root->R);
        displayT1(Root->RC);
    }
}

void displayIndexFile(char *filename)
{
    t_TOF *f;
    t_block Buf;

    TOF_open(&f, filename, 'E');
    int nBlocks = getHeader(f, "Nblocks");
    printf("\nIndex file contains %d blocks\n", nBlocks);

    for (int i = 1; i <= nBlocks; i++) {
        TOF_readBlock(f, i, &Buf);
        printf("\nBlock %d (%d records):\n", i, Buf.Nrec);
        for (int j = 0; j < Buf.Nrec; j++) {
            printf("  Key=%d | B=%d | R=%d\n",
                   Buf.Tab[j].Key,
                   Buf.Tab[j].blkAddr,
                   Buf.Tab[j].recAddr);
        }
    }
    TOF_close(f);
}

/* ------------------ B-tree display helper ------------------ */
void printBTree(B_Tree *Node, int level)
{
    if (!Node) return;
    for (int i = Node->degree - 1; i >= 0; i--) {
        printBTree(Node->child[i], level + 1);
        if (i < Node->degree - 1)
            printf("%*sKey[%d]=%d\n", level * 4, "", i, Node->Key[i]);
    }
}

/* ------------------ Main menu ------------------ */
int main()
{
    t_TOF *IndexFile = NULL;
    t_T1 *RootT1 = NULL;
    B_Tree *BRoot = NULL;

    int choice;
    int key, blk, rec;
    int found;

    char filename[50] = "output.tof";

    do {
        printf("\n==================== MENU ====================\n");
        printf("1. Load T1/T2 from index file\n");
        printf("2. Save index from T1/T2 to file\n");
        printf("3. Search a key\n");
        printf("4. Insert a key\n");
        printf("5. Split a node in B-tree\n");
        printf("6. Display T1/T2 trees (RAM)\n");
        printf("7. Display index file (TOF)\n");
        printf("0. Exit\n");
        printf("==============================================\n");
        printf("Your choice: ");
        scanf("%d", &choice);

        switch (choice) {

        case 1: /* Load T1/T2 from TOF */
            TOF_open(&IndexFile, filename, 'E');
            LoadTreeFromFile(IndexFile, &RootT1);
            TOF_close(IndexFile);
            printf("✔ Index loaded into T1/T2 successfully\n");
            break;

        case 2: /* Save T1/T2 to TOF */
            TOF_open(&IndexFile, filename, 'N');
            SeveTreeToFile(IndexFile, RootT1);
            TOF_close(IndexFile);
            printf("✔ T1/T2 saved to index file\n");
            break;

        case 3: /* Search */
            if (RootT1 == NULL) {
                printf("✖ Tree is empty. Load or insert first.\n");
                break;
            }
            printf("Enter key to search: ");
            scanf("%d", &key);
            searchKey(RootT1, key, &found, &blk, &rec);
            if (found)
                printf("✔ Key found → Block: %d | Record: %d\n", blk, rec);
            else
                printf("✖ Key not found\n");
            break;

        case 4: /* Insert */
            if (RootT1 == NULL) createTree(&RootT1);
            printf("Enter key: ");
            scanf("%d", &key);
            printf("Enter block number: ");
            scanf("%d", &blk);
            printf("Enter record number: ");
            scanf("%d", &rec);
            insertInTree(&RootT1, key, blk, rec);
            printf("✔ Key inserted successfully\n");
            break;

       case 5: /* B-tree split */
        if (BRoot == NULL) {
            allocateBTreeNode(&BRoot);
            // Full leaf node
            BRoot->Key[0] = 10;
            BRoot->Key[1] = 20;
            BRoot->Key[2] = 30;
            BRoot->Key[3] = 40;
            BRoot->degree = 5;
        }

        printf("\nCurrent B-tree before insertion:\n");
        printBTree(BRoot, 0);

        printf("\nEnter key to insert into full leaf: ");
        scanf("%d", &key);

        splitLeafNode(&BRoot, BRoot, key);

        printf("\nB-tree after inserting %d:\n", key);
        printBTree(BRoot, 0);

        printf("✔ B-tree split executed (if needed)\n");
        break;
        case 6: /* Display T1/T2 */
            if (RootT1 == NULL) printf("✖ Tree is empty\n");
            else {
                printf("\n======= T1 / T2 TREE CONTENT =======\n");
                displayT1(RootT1);
                printf("\n===================================\n");
            }
            break;

        case 7: /* Display index file */
            displayIndexFile(filename);
            break;

        case 0:
            printf("Exiting program...\n");
            break;

        default:
            printf("✖ Invalid choice\n");
        }

    } while (choice != 0);

    /* Cleanup */
    freeTree(&RootT1);

    return 0;
}
