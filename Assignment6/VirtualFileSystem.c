#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_BUFFER_LENGTH 524288                  // Max command input length
#define MAX_STRING_LENGTH 50                    // Max dir / file length
#define MAX_FILE_BLOCKS 100                     // Max blocks per file
#define BLOCK_SIZE 512
#define NUM_OF_BLOCKS 1024

struct FileNode{
    char name[MAX_STRING_LENGTH];               // name of dir or file
    bool isDirectory;                           //directory or file 
    int blockPointers[MAX_FILE_BLOCKS];          //to store block indices
    int numBlocks;                              // count the number of blocks used
    int size;                                   // size of file (content) 
    struct FileNode *parent;                    // parent 
    struct FileNode *child;                     // child each directory's children form a circular list
    struct FileNode *next;                      // next sibling also a circular Linked lIst
};

char virtualDisk[NUM_OF_BLOCKS][BLOCK_SIZE];    // 1024 blocks, each 512 bytes , total 512KB or 0.5 MB 

struct FreeBlockNode{
    int index;                              //block number0 - 1023
    struct FreeBlockNode *next;
    struct FreeBlockNode *prev;
};

struct FileNode *root = NULL;
struct FileNode *current = NULL;            //always points ot cwd
struct FreeBlockNode *blockListHead = NULL;
struct FreeBlockNode *blockListTail = NULL;

void initializeRootDirectory(){
    root = (struct FileNode*)malloc(sizeof(struct FileNode));
    if(root == NULL){
        printf("Memory allocation failed.");
        return;
    }
    strcpy(root->name, "/");
    root->isDirectory = true;
    root->next = NULL;
    root->parent =NULL;
    root->child = NULL;
    root->numBlocks = 0;
    root->size = 0;
    current = root;  // initally cwd is at root 
    return;
}

void initializeFreeBlocks(){
    for(int i =0 ; i < NUM_OF_BLOCKS ; i++){
        struct FreeBlockNode *newBlock = (struct FreeBlockNode *)malloc(sizeof(struct FreeBlockNode));
        if(newBlock == NULL){
            printf("Memory allocation Failed");
            exit(1);
        }
        newBlock->index = i;
        newBlock->next = NULL;
        newBlock->prev = blockListTail;
        if(blockListHead == NULL){      // First block index 0
            blockListHead = newBlock; 
        }else{
            blockListTail->next = newBlock;  // link to previous last block
        }
        blockListTail = newBlock; // update tail keep trace
    }
    return;
}

void printPrompt(){
    printf("\n%s > ", current->name);  // evrytime prompt 
    return;
}

const char *validCommands[] =  {"mkdir", "rmdir", "pwd", "cwd", "df", "ls", "create", "read", "write", "delete", "cd"};

int getCommandIndex(const char *cmd) {
    int numberOfCommands = sizeof(validCommands) / sizeof(validCommands[0]);
    for (int i = 0; i < numberOfCommands; i++) {
        if (strcmp(cmd, validCommands[i]) == 0)
            return i;
    }
    return -1;
}

void trimSpaces(char *str) {
    if (str == NULL) return;
    int start = 0, end = strlen(str) - 1;
    while (isspace((unsigned char)str[start])) start++;
    while (end >= start && isspace((unsigned char)str[end])) end--;
    memmove(str, str + start, end - start + 1);
    str[end - start + 1] = '\0';
}

bool isValidName(char *name) {
    if (name == NULL) return false;
    trimSpaces(name);
    if (strlen(name) == 0)  
        return false;
    for (int i = 0; name[i]; i++) {
        if (strchr("\\/*?:\"<>|", name[i]) != NULL) // invalid char
            return false;
    }
    return true;
}

void mkdirPerform(char *directoryName){
    if (directoryName == NULL) {
        printf("Error: Directory name required.\n");
        return;
    }
    if (!isValidName(directoryName)) {
        printf("Error: Invalid or empty directory name.\n");
        return;
    }
    
    // check for dupliacte used circular traverse
    if (current->child != NULL) {
        struct FileNode *temp = current->child;
        struct FileNode *start = temp;
        
        do {
            if (strcmp(temp->name, directoryName) == 0 && temp->isDirectory) {
                printf("Error: Directory '%s' already exists.\n", directoryName);
                return;
            }
            temp = temp->next;
        } while (temp != start);  //circular termination
    }
    
    struct FileNode *newDirectory = (struct FileNode*)malloc(sizeof(struct FileNode));
    if(newDirectory == NULL){
        printf("Memory allocation fail");
        exit(1);
    }
    strcpy(newDirectory->name, directoryName);
    newDirectory->isDirectory = true;
    newDirectory->parent = current;
    newDirectory->child = NULL; 
    newDirectory->next = NULL; 
    
    // circular Insert new node and maintaining circular structure
    if (current->child == NULL) { // first child 
        current->child = newDirectory;
        newDirectory->next = newDirectory;  // Points to itself (self circular)
    } else {
        struct FileNode *temp2 = current->child;
        while (temp2->next != current->child){  //find last node in circular list
            temp2 = temp2->next;
        }
        newDirectory->next = current->child;  // Point to first child
        temp2->next = newDirectory;           // Last points to new node
    }
    printf("Directory '%s' created successfully.",directoryName);
    return;
}

void rmdirPerform(char *dirName) {
    if (dirName == NULL) {
        printf("Error: Please provide a directory name.\n");
        return;
    }
    if(strcmp(dirName, root->name) == 0){
        printf("You can not delete the root directory.");
        return;
    }
    
    if (current->child == NULL) {
        printf("Error: Directory '%s' not found.\n", dirName);
        return;
    }
    
    struct FileNode *temp = current->child;
    struct FileNode *prev = NULL;

    if (temp->next == temp) {  // only one child
        if (strcmp(temp->name, dirName) == 0) {
            if (temp->child != NULL) {
                printf("Error: Directory '%s' is not empty.\n", dirName);
                return;
            }
            current->child = NULL;      // no child left
            free(temp);              //only remove empty directory
            printf("Directory '%s' removed successfully.\n", dirName);
            return;
        }
    } 
    // if multiple children used circular traversal
    struct FileNode *start = temp;
    do {
        if (temp->isDirectory && strcmp(temp->name, dirName) == 0) {
            
            if (temp->child != NULL) { 
                printf("Error: Directory '%s' is not empty.\n", dirName);
                return;
            }
            
            if (prev == NULL) {
                current->child = temp->next;
            }
            
            prev->next = temp->next;   // no child left
            free(temp);  //only remove empty directory
            printf("Directory '%s' removed successfully.\n", dirName);
            return;
        }
        prev = temp;
        temp = temp->next;
    } while (temp != start);

    printf("Error: Directory '%s' not found.\n", dirName);
}

void lsPerform() {
    if (current->child == NULL) {
        printf("(empty)\n");
        return;
    }

    struct FileNode *temp = current->child;
    struct FileNode *start = temp;

    do {
        printf("%s%s  \n", temp->name, temp->isDirectory ? "/" : "");  //    / suffix for directories
        temp = temp->next;
    } while (temp != start);  // Stop when we loop back to start circular traversal

}

void cwdPerform() {
    printf("Current Directory: %s\n", current->name);
}

void pwdPerform() {
    if (current == NULL) {
        printf("Error: No current directory.\n");
        return;
    }
    char paths[50][MAX_STRING_LENGTH];
    int count = 0;
    struct FileNode *temp = current;
    while (temp != NULL) {   // collect path by following parent pointers
        strcpy(paths[count], temp->name);
        count++;
        temp = temp->parent;
    }
    for (int i = count - 1; i >= 0; i--) {      // Print path in reverse (from root to current)
        if (i == count - 1 && strcmp(paths[i], "/") == 0)
            printf("/");
        else
            printf("%s%s", (strcmp(paths[i], "/") == 0) ? "" : "/", paths[i]);
    }
    printf("\n");
}

void dfPerform() {
    int totalBlocks = NUM_OF_BLOCKS;
    int usedBlocks = 0;
    struct FreeBlockNode *temp = blockListHead;
    int freeBlocks = 0;
    while (temp != NULL) {  // count free blocks 
        freeBlocks++;
        temp = temp->next;
    }
    usedBlocks = totalBlocks - freeBlocks;
    double usagePercent = ((double)usedBlocks / totalBlocks) * 100.0;
    printf("Total Blocks: %d\n", totalBlocks);
    printf("Used Blocks: %d\n", usedBlocks);
    printf("Free Blocks: %d\n", freeBlocks);
    printf("Disk Usage: %.2f%%\n", usagePercent);
}

void cdPerform(char *dirName) {
    if (dirName == NULL) {
        printf("Error: Please provide a directory name.\n");
        return;
    }

    if (strcmp(dirName, "..") == 0) { // going to parent directory
        if (current->parent != NULL) {
            current = current->parent;    // Update current to parent
            printf("Moved to %s\n", current->name); 
        } else {
            printf("Already at root.\n");
        }
        return;
    }
    // going to child directory 
    if (current->child != NULL) {
        struct FileNode *temp = current->child;
        struct FileNode *start = temp;
        do {
            if (temp->isDirectory && strcmp(temp->name, dirName) == 0) {
                current = temp; // // Update current to child
                printf("Moved to /%s\n", temp->name);
                return;
            }
            temp = temp->next;
        } while (temp != start);
    }
    printf("Error: Directory '%s' not found.\n", dirName);
}

void createPerform(char *fileName) {
    if (fileName == NULL || !isValidName(fileName)) {
        printf("Error: Invalid file name.\n");
        return;
    }
    // validatation file name should be unique into the same dir 
    if (current->child != NULL) {
        struct FileNode *temp = current->child;
        struct FileNode *start = temp;
        
        do {
            if (strcmp(temp->name, fileName) == 0) {
                printf("Error: File or directory '%s' already exists.\n", fileName);
                return;
            }
            temp = temp->next;
        } while (temp != start);
    }

    struct FileNode *newFile = (struct FileNode *)malloc(sizeof(struct FileNode));
    if (newFile == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    strcpy(newFile->name, fileName);
    newFile->isDirectory = false;
    newFile->numBlocks = 0;  // no block allocated yet 
    newFile->size = 0;
    newFile->parent = current;
    newFile->child = NULL;
    newFile->next = NULL;
    // circular 
    if (current->child == NULL) {
        current->child = newFile;
        newFile->next = newFile;  // Points to itself
    } else {
        struct FileNode *temp2 = current->child;
        while (temp2->next != current->child){
            temp2 = temp2->next;
        }
        newFile->next = current->child;  // Point to first child
        temp2->next = newFile;  // Last points to new node
    }
    printf("File '%s' created successfully.\n", fileName);
}

void writePerform(char *fileName, char *data) {
    if (fileName == NULL) { // validate Input
        printf("Error: File name required.\n");
        return;
    }
    trimSpaces(fileName);  
    
    if (data == NULL || strlen(data) == 0) {
        printf("Error: No data provided to write.\n");
        return;
    }
    
    if (data == NULL) data = ""; 
    trimSpaces(data);

    int len = strlen(data);  
    if (len >= 2 && data[0] == '"' && data[len - 1] == '"') {  //Remove 1st and last quotes from data
        data[len - 1] = '\0';
        data++;
    }
    if (current->child == NULL) {  // find the file into the dir 
        printf("Error: File '%s' not found.\n", fileName);
        return;
    }

    struct FileNode *temp = current->child;  // start at first child
    struct FileNode *start = temp;             // Remember where we started

    do {
        if (!temp->isDirectory && strcmp(temp->name, fileName) == 0) {  // check for FILE Found then do the write operation
             for (int i = 0; i < temp->numBlocks; i++) {  // free old blocks if rewrite
                struct FreeBlockNode *blk = malloc(sizeof(struct FreeBlockNode));
                blk->index = temp->blockPointers[i];
                blk->next = NULL;
                blk->prev = blockListTail;
                if (blockListTail) blockListTail->next = blk;
                blockListTail = blk;
                if (!blockListHead) blockListHead = blk;
            }
            int dataSize = strlen(data); // new content size 
            int blocksNeeded = (dataSize + BLOCK_SIZE - 1) / BLOCK_SIZE; 
            if (blocksNeeded > MAX_FILE_BLOCKS) {
                printf("Error: File too large.\n");
                return;
            }
            temp->numBlocks = 0;  // reset block count 
            for (int i = 0; i < blocksNeeded; i++) { //alloactes blocks from free 
                if (blockListHead == NULL) {
                    printf("Error: Disk full.\n");
                    return;
                }
                struct FreeBlockNode *block = blockListHead; // get first free block
                temp->blockPointers[temp->numBlocks++] = block->index;  //assign this to file

                blockListHead = block->next; //move head to next free block
                if (blockListHead)
                    blockListHead->prev = NULL;
                else
                    blockListTail = NULL;
                free(block);
            }

            //write data into the allocated blovks
            int offset = 0;
            for (int i = 0; i < temp->numBlocks; i++) {
                int bytes = (dataSize - offset > BLOCK_SIZE) ? BLOCK_SIZE : (dataSize - offset);
                memcpy(virtualDisk[temp->blockPointers[i]], data + offset, bytes);   // copy data to virtual disk block
                if (bytes < BLOCK_SIZE)
                    virtualDisk[temp->blockPointers[i]][bytes] = '\0'; // Add null terminator if this is the last block
                offset += bytes;
            }
            temp->size = dataSize; //store file size for read operations
            printf("Data written to '%s' (%d bytes).\n", fileName, dataSize);
            return;
        }
        temp = temp->next;
    } while (temp != start); // stop when we are at the satrt mena circle complete 

    printf("Error: File '%s' not found.\n", fileName);
}

void readPerform(char *fileName) {
    if (fileName == NULL) { //user ran command without argument
        printf("Error: File name required.\n");
        return;
    }
    if (current->child != NULL) {  // if current directory is not empty then we can search for the file
        struct FileNode *temp = current->child; // start from 1st child 
        struct FileNode *start = temp;          // remember the 1st child to stop the loop after a cycle
        
        do {
            if (!temp->isDirectory && strcmp(temp->name, fileName) == 0) {  //should be file and name should be same 
                if (temp->size == 0) {
                    printf("File '%s' is empty.\n", fileName);
                    return;
                }
                int remaining = temp->size;
                // read and display its content
                for (int i = 0; i < temp->numBlocks && remaining > 0; i++) {
                    int bytes = (remaining > BLOCK_SIZE) ? BLOCK_SIZE : remaining;
                    fwrite(virtualDisk[temp->blockPointers[i]], 1, bytes, stdout); //copies data from memory to output stream
                    remaining -= bytes;
                }
                printf("\n");
                return;
            }
            temp = temp->next;
        } while (temp != start); // stops when we loop back to start
    }
    printf("Error: File '%s' not found.\n", fileName);

}
void deletePerform(char *fileName) {
    if (fileName == NULL) {
        printf("Error: File name required.\n"); // if user run command with out filename 
        return;
    }
    if (current->child == NULL) {    // if current directory is not empty then we can search for the file
        printf("Error: File '%s' not found.\n", fileName);
        return;
    }
    struct FileNode *temp = current->child;  
    struct FileNode *prev = NULL;
    struct FileNode *start = temp;
    do {
        if (!temp->isDirectory && strcmp(temp->name, fileName) == 0) {
            // Free all blocks allocated to this file
            for (int i = 0; i < temp->numBlocks; i++) {
                struct FreeBlockNode *blk = malloc(sizeof(struct FreeBlockNode));
                blk->index = temp->blockPointers[i];
                blk->next = NULL;
                blk->prev = blockListTail;
                if (blockListTail) blockListTail->next = blk;
                blockListTail = blk;
                if (!blockListHead) blockListHead = blk;
            }
            
            if (temp->next == temp) {  // only one child mean only this node no others
                current->child = NULL;
                free(temp);
                printf("File '%s' deleted successfully.\n", fileName);
                return;
            }
            
            //multiple nodes in circular list
            if (prev == NULL) { // Deleting first node
                current->child = temp->next;
                struct FileNode *last = temp->next;
                while (last->next != temp) {
                    last = last->next;
                }
                last->next = current->child;  //maintain circular!
            } else { // Deleting non-first node
                prev->next = temp->next; // circular already maintained by prev pointing to temp->next
            }
            free(temp); 
            printf("File '%s' deleted successfully.\n", fileName);
            return;
        }
        
        prev = temp;
        temp = temp->next;
    } while (temp != start);
    
    printf("Error: File '%s' not found.\n", fileName);
}

void execute(char *input){
    if(input == NULL) return;
    char *inputCommand = strtok(input," ");
    int cmdIndex = getCommandIndex(inputCommand);
    if (cmdIndex == -1) {
        printf("Invalid Command");
        return;
    }
    switch (cmdIndex) {
        case 0: mkdirPerform(strtok(NULL, "")); break;
        case 1: rmdirPerform(strtok(NULL, "")); break;
        case 2: pwdPerform(); break;
        case 3: cwdPerform(); break;
        case 4: dfPerform(); break;
        case 5: lsPerform(); break;
        case 6: createPerform(strtok(NULL, "")); break;
        case 7: readPerform(strtok(NULL, "")); break;
        case 8:{
                char *fileName = strtok(NULL, " ");
                char *data = strtok(NULL, "");  
                writePerform(fileName, data);
                break;
        }
        case 9: deletePerform(strtok(NULL, "")); break;
        case 10: cdPerform(strtok(NULL, "")); break;
        default: printf("Something went wrong.\n");
    }
}    

void freeDirectoryTree(struct FileNode *node) {  //Recursively free entire directory tree
    if (node == NULL) { 
        return;
    }
    if (node->child != NULL) {  
        struct FileNode *temp = node->child;
        struct FileNode *start = temp;
        do {
            struct FileNode *next = temp->next; 
            
            freeDirectoryTree(temp); // recursive call
            
            temp = next;
        } while (temp != start); 
    }
    free(node);
}

int main() {

    printf("$ ./vfs\nCompact VFS - ready. Type 'exit' to quit.");

    initializeRootDirectory();
    initializeFreeBlocks();

    char *input= malloc(MAX_BUFFER_LENGTH * sizeof(char));
    if(input== NULL){
        printf("Memory allocation failed");
        return 1;
    }

    while (1) {
        printPrompt();
        if (fgets(input, MAX_BUFFER_LENGTH, stdin) == NULL) break;
        input[strcspn(input, "\n")] = '\0';
        if (strlen(input) == 0)
            continue;
        if (strcmp(input, "exit") == 0)
            break;
        execute(input);
    }

    free(input);
    struct FreeBlockNode *temp = blockListHead;
    while (temp) {
        struct FreeBlockNode *next = temp->next;
        free(temp);
        temp = next;
    }
    freeDirectoryTree(root);
    printf("Memory released. Exiting program...\n"); 

    return 0;
}