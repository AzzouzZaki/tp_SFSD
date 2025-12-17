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
void printBTree(B_Tree *node, int level)
{
    if (node == NULL) return;

    printf("Level %d: [", level);

    for (int i = 0; i < node->degree - 1; i++)
    {
        printf("%d", node->Key[i]);
        if (i < node->degree - 2) printf(" ");
    }
    printf("]\n");

    /* Print children */
    for (int i = 0; i < node->degree; i++)
    {
        printBTree(node->child[i], level + 1);
    }
}




// Recursive function to create a full B-tree of given height
B_Tree* createFullBTree(int height, int* keyCounter, B_Tree** fullLeaf) {
    int n = 100; // range for random keys
    if (height <= 0) return NULL;

    B_Tree* node = NULL;
    allocateBTreeNode(&node);

    // Fill this node with max keys (4)
    for (int i = 0; i < 4; i++) {
        node->Key[i] = (*keyCounter)++;
         int r = rand() % (n + 1);
        (*keyCounter)= (*keyCounter) + r + 1; // ensure increasing keys
    }
    node->degree = 5; // max keys + 1 children

    if (height == 1) {
        // This is a leaf
        *fullLeaf = node; // keep track of the deepest full leaf
        return node;
    }

    // Create 5 children recursively
    for (int i = 0; i < 5; i++) {
        node->child[i] = createFullBTree(height - 1, keyCounter, fullLeaf);
        if (node->child[i] != NULL)
            node->child[i]->parent = node;
    }

    return node;
}

// Wrapper function
B_Tree* buildBTreeWithHeight(int height, B_Tree** fullLeaf) {
    int keyCounter = 10; // starting key
    return createFullBTree(height, &keyCounter, fullLeaf);
}

// Simple printer for B-tree levels
void printBTreeLevels(B_Tree* root, int level) {
    if (!root) return;

    printf("Level %d: [", level);
    for (int i = 0; i < root->degree - 1; i++) {
        printf("%d ", root->Key[i]);
    }
    printf("]\n");

    for (int i = 0; i < root->degree; i++) {
        printBTreeLevels(root->child[i], level + 1);
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

       /* -------------------------------------------------- */
        case 5: /* B-tree split (create full tree and split a leaf) */
        {
            int h, key;
            printf("Enter desired height of B-tree: ");
            scanf("%d", &h);

            B_Tree* fullLeaf = NULL;
            BRoot = buildBTreeWithHeight(h, &fullLeaf);

            if (fullLeaf == NULL) {
                printf("✖ Could not create full leaf node\n");
                break;
            }

            printf("\nCurrent B-tree before insertion:\n");
            printBTreeLevels(BRoot, 0);

            printf("\nEnter key to insert into full leaf: ");
            scanf("%d", &key);

            int middleValue;
            B_Tree *newLeft = NULL, *newRight = NULL;

            splitLeafNode(&BRoot, fullLeaf, key, &middleValue, &newLeft, &newRight);

            printf("\nInserted key %d into full leaf (middle value to promote: %d)\n", key, middleValue);
            printf("New Left Node Keys: [");
            for(int i = 0; i < newLeft->degree - 1; i++){
                printf("%d ", newLeft->Key[i]);
            }
            printf("]\n");
            printf("New Right Node Keys: [");
            for(int i = 0; i < newRight->degree - 1; i++){
                printf("%d ", newRight->Key[i]);
            }
            printf("]\n");
        }
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
