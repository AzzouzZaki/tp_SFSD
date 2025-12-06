#include "header.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    t_TOF *inputFile, *outputFile;
    t_T1 *tree = NULL;
    
    // Create a new tree and insert some test data
    createTree(&tree);
    
    printf("Inserting test data into tree...\n");
    
    // Insert some sample data
    // Format: insertInTree(&tree, Key, BlockNumber, RecordNumber)
    insertInTree(&tree, 50, 1, 1);
    insertInTree(&tree, 30, 1, 2);
    insertInTree(&tree, 70, 1, 3);
    insertInTree(&tree, 20, 2, 1);
    insertInTree(&tree, 40, 2, 2);
    insertInTree(&tree, 60, 2, 3);
    insertInTree(&tree, 80, 3, 1);
    insertInTree(&tree, 10, 3, 2);
    insertInTree(&tree, 90, 3, 3);
    insertInTree(&tree, 55, 4, 1);
    insertInTree(&tree, 65, 4, 2);
    insertInTree(&tree, 75, 4, 3);
    insertInTree(&tree, 85, 5, 1);
    
    // Create a new TOF file to save the tree
    printf("Creating output file...\n");
    TOF_open(&outputFile, "output.tof", 'N');
    
    // Save the tree to file
    printf("Saving tree to file...\n");
    SeveTreeToFile(outputFile, tree);
    
    // Close the output file
    TOF_close(outputFile);
    
    // Now let's verify by loading the file back
    printf("\nVerifying by loading the saved file...\n");
    TOF_open(&inputFile, "output.tof", 'E');
    
    t_T1 *loadedTree = NULL;
    LoadTreeFromFile(inputFile, &loadedTree);
    
    // Get number of blocks from header
    long numBlocks = getHeader(inputFile, "Nblocks");
    printf("File contains %ld blocks\n", numBlocks);
    
    // Read and display each block
    t_block buffer;
    for (long i = 0; i < numBlocks; i++) {
        TOF_readBlock(inputFile, i + 1, &buffer);
        printf("\nBlock %ld (contains %d records):\n", i + 1, buffer.Nrec);
        for (int j = 0; j < buffer.Nrec; j++) {
            printf("  Record %d: Key=%d, BlockAddr=%d, RecordAddr=%d\n", 
                   j + 1, 
                   buffer.Tab[j].Key, 
                   buffer.Tab[j].blkAddr, 
                   buffer.Tab[j].recAddr);
        }
    }
    
    // Clean up
    TOF_close(inputFile);
    freeTree(&tree);
    freeTree(&loadedTree);
    
    printf("\nTest completed successfully!\n");
    
    return 0;
}
