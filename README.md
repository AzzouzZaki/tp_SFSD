# TP SFSD - Hierarchical Index with T1/T2 Trees

## Description

This project implements a hierarchical index structure using two levels of binary search trees (T1 and T2) to efficiently manage a TOF (Table Ordered File) organization file. The structure allows for fast search, insertion, and saving of records.

## Project Structure

```
tp_SFSD/
├── header.h          # Structure declarations and prototypes
├── header.c          # Main function implementations
├── main.c            # Main program with tests
├── testLoad.c        # Loading tests
├── testSave.c        # Saving tests
├── data.tof          # Sample data file
├── output.tof        # Output file
├── test.tof          # Test file
└── bin/              # Executables directory
```

## Compilation

### Manual Compilation

```bash
gcc -o bin/main main.c header.c -Wall -Wextra
```

### Test Compilation

```bash
# Loading test
gcc -o bin/testLoad testLoad.c header.c -Wall -Wextra

# Saving test
gcc -o bin/testSave testSave.c header.c -Wall -Wextra
```

## Usage

### 1. Main Program Execution

```bash
./bin/main
```

The main program allows you to:
- Create a new index
- Load an index from a file
- Insert records
- Search for records by key
- Save the index to a file
- Display the index structure

### 2. Interactive Menu

After execution, a menu is displayed with the following options:

```
1. Load index from file
2. Insert a record
3. Search for a record
4. Save index
5. Display tree in memory
6. Display index file
7. Quit
```

### 3. Usage Examples

#### Create and Save an Index

1. Launch the program
2. Choose option **2** to insert records
3. Enter the information: Key, Block number, Record number
4. Repeat for multiple records
5. Choose option **4** to save to `output.tof`

#### Load an Existing Index

1. Launch the program
2. Choose option **1** to load from a file
3. The index will be loaded into RAM

#### Search for a Record

1. Choose option **3**
2. Enter the key to search for
3. The program displays:
   - If record is found: block and record number
   - Otherwise: "Key not found" message

#### Display Structure

- **Option 5**: Display T1/T2 tree in memory
- **Option 6**: Display index file contents block by block

## TOF File Format

### Structure

- **Header**: Contains the number of blocks (4 bytes)
- **Blocks**: Each block contains up to MAXTAB (10) records

### Record

Each record contains:
- `Key`: Record key (integer)
- `blkAddr`: Data block address (integer)
- `recAddr`: Record address in block (integer)

## Data Structures

### T1 Tree

First-level structure that partitions the key space:
- `V1` : Minimum value in the subtree
- `V2` : Maximum value in the subtree
- `LC` : Pointer to left child
- `RC` : Pointer to right child
- `R` : Pointer to T2 tree root

### T2 Tree

Second-level structure containing records:
- `Key` : Record key
- `BlkNum` : Block number
- `RecNum` : Record number
- `LC` : Pointer to left child
- `RC` : Pointer to right child
## Tests

### Loading Test

```bash
./bin/testLoad
```

Loads an index from `data.tof` and displays its structure.

### Saving Test

```bash
./bin/testSave
```

Creates an index, inserts records and saves it to `output.tof`.

## Main Functionalities

### Hierarchical Index (T1/T2)

```c
// File Operations
t_TOF *f;
TOF_open(&f, "filename.tof", 'N');  // 'N' for new, 'E' for existing
TOF_close(f);

// Loading & Saving
t_T1 *Root;
LoadTreeFromFile(f, &Root);
SeveTreeToFile(f, Root);

// Insertion & Search
insertInTree(&Root, key, blockNum, recordNum);

int found, blkNum, recNum;
searchKey(Root, key, &found, &blkNum, &recNum);
```

### B-Tree Operations

```c
// Leaf Node Splitting - Splits a full leaf (4 keys) by promoting median to parent
int middleValue;
B_Tree *newLeft, *newRight;
splitLeafNode(&Root, fullLeafNode, newKey, &middleValue, &newLeft, &newRight);

// Node Verification - Checks if node is a leaf
if (checkIfLeafNode(node)) { /* ... */ }

// Node Allocation - Allocates and initializes a new B-Tree node
B_Tree *newNode;
allocateBTreeNode(&newNode);
```

## Authors

* **Azzouz Zaki**
* **Amara Khaled Walid**

**Supervised by**: *Ens Mohamed Lamine Chouder*

## 📄 License
This project is part of an academic exercise and is not licensed for commercial use.
This work is property of the Higher School of Computer Science Algiers (ESI Algiers).
