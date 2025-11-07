#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_BUFFER_LENGTH 512
#define MAX_STRING_LENGTH 50
#define BLOCK_SIZE 512
#define NUM_OF_BLOCKS 1024
#define MAX_FILE_BLOCKS 100

struct FileNode{
    char name[MAX_STRING_LENGTH];
    bool isDirectory; //directory or file 
   int blockPointers[MAX_FILE_BLOCKS];  // Array to store block indices
    int numBlocks;               // Number of blocks used
    int size;                    
    struct FileNode *parent;    // parent 
    struct FileNode *child;     // child 
    struct FileNode *next;      // sibling
};

char virtualDisk[NUM_OF_BLOCKS][BLOCK_SIZE];

struct FreeBlockNode{
    int index;
    struct FreeBlockNode *next;
    struct FreeBlockNode *prev;
};

struct FileNode *root = NULL;
struct FileNode *current = NULL;
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
    current = root;
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
        if(blockListHead == NULL){
            blockListHead = newBlock;
        }else{
            blockListTail->next = newBlock;
        }
        blockListTail = newBlock;
    }
    return;
}

void printPrompt(){
    printf("\n%s > ", current->name);
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
        if (strchr("\\/*?:\"<>|", name[i]) != NULL)
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
    struct FileNode *temp = current->child;
    while (temp != NULL) {
        if (strcmp(temp->name, directoryName) == 0 && temp->isDirectory) {
            printf("Error: Directory '%s' already exists.\n", directoryName);
            return;
        }
        temp = temp->next;
    }
    struct FileNode *newDirectory = (struct FileNode*)malloc(sizeof(struct FileNode));
    if(newDirectory == NULL){
        printf("Memory alloaction fail");
        exit(1);
    }
    strcpy(newDirectory->name, directoryName);
    newDirectory->isDirectory = true;
    newDirectory->parent = current;
    newDirectory->child = NULL; 
    newDirectory->next = NULL; 
    if (current->child == NULL) { // means there is no child of this current directory
        current->child = newDirectory;
    } else {
        struct FileNode *temp2 = current->child;
        while (temp2->next != NULL){ 
            temp2 = temp2->next;
        }
        temp2->next = newDirectory;
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
    struct FileNode *temp = current->child;
    struct FileNode *prev = NULL;
    while (temp != NULL) {
        if (temp->isDirectory && strcmp(temp->name, dirName) == 0) {
            if (temp->child != NULL) {
                printf("Error: Directory '%s' is not empty.\n", dirName);
                return;
            }
            if (prev == NULL) {
                current->child = temp->next;
            } else {
                prev->next = temp->next;
            }
            free(temp);
            printf("Directory '%s' removed successfully.\n", dirName);
            return;
        }
        prev = temp;
        temp = temp->next;
    }
    printf("Error: Directory '%s' not found.\n", dirName);
}

void lsPerform() {
    if (current->child == NULL) {
        printf("(empty)\n");
        return;
    }
    struct FileNode *temp = current->child;
    while (temp != NULL) {
        printf("%s%s  \n", temp->name, temp->isDirectory ? "/" : "");
        temp = temp->next;
    }
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
    while (temp != NULL) {
        strcpy(paths[count], temp->name);
        count++;
        temp = temp->parent;
    }
    for (int i = count - 1; i >= 0; i--) {
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
    while (temp != NULL) {
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
    if (strcmp(dirName, "..") == 0) {
        if (current->parent != NULL)
            current = current->parent;
        else
            printf("Already at root.\n");
        return;
    }
    struct FileNode *temp = current->child;
    while (temp != NULL) {
        if (temp->isDirectory && strcmp(temp->name, dirName) == 0) {
            current = temp;
            return;
        }
        temp = temp->next;
    }
    printf("Error: Directory '%s' not found.\n", dirName);
}

void createPerform(char *fileName) {
    if (fileName == NULL || !isValidName(fileName)) {
        printf("Error: Invalid file name.\n");
        return;
    }
    struct FileNode *temp = current->child;
    while (temp != NULL) {
        if (strcmp(temp->name, fileName) == 0) {
            printf("Error: File or directory '%s' already exists.\n", fileName);
            return;
        }
        temp = temp->next;
    }
    struct FileNode *newFile = (struct FileNode *)malloc(sizeof(struct FileNode));
    if (newFile == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    strcpy(newFile->name, fileName);
    newFile->isDirectory = false;
    newFile->numBlocks = 0;
    newFile->size = 0;
    newFile->parent = current;
    newFile->child = NULL;
    newFile->next = NULL;
    if (current->child == NULL)
        current->child = newFile;
    else {
        struct FileNode *t = current->child;
        while (t->next != NULL)
            t = t->next;
        t->next = newFile;
    }
    printf("File '%s' created successfully.\n", fileName);
}

void writePerform(char *fileName, char *data) {
    if (fileName == NULL) {
        printf("Error: File name required.\n");
        return;
    }
    trimSpaces(fileName);
    if (data == NULL) data = "";
    trimSpaces(data);

    int len = strlen(data);
    if (len >= 2 && data[0] == '"' && data[len - 1] == '"') {
        data[len - 1] = '\0';
        data++;
    }
    struct FileNode *temp = current->child;
    while (temp != NULL) {
        if (!temp->isDirectory && strcmp(temp->name, fileName) == 0) {
            for (int i = 0; i < temp->numBlocks; i++) {
                struct FreeBlockNode *blk = malloc(sizeof(struct FreeBlockNode));
                blk->index = temp->blockPointers[i];
                blk->next = NULL;
                blk->prev = blockListTail;
                if (blockListTail) blockListTail->next = blk;
                blockListTail = blk;
                if (!blockListHead) blockListHead = blk;
            }
            int dataSize = strlen(data);
            int blocksNeeded = (dataSize + BLOCK_SIZE - 1) / BLOCK_SIZE;
            if (blocksNeeded > MAX_FILE_BLOCKS) {
                printf("Error: File too large.\n");
                return;
            }
            temp->numBlocks = 0;
            for (int i = 0; i < blocksNeeded; i++) {
                if (blockListHead == NULL) {
                    printf("Error: Disk full.\n");
                    return;
                }
                struct FreeBlockNode *block = blockListHead;
                temp->blockPointers[temp->numBlocks++] = block->index;

                blockListHead = block->next;
                if (blockListHead)
                    blockListHead->prev = NULL;
                else
                    blockListTail = NULL;
                free(block);
            }
            int offset = 0;
            for (int i = 0; i < temp->numBlocks; i++) {
                int bytes = (dataSize - offset > BLOCK_SIZE) ? BLOCK_SIZE : (dataSize - offset);
                memcpy(virtualDisk[temp->blockPointers[i]], data + offset, bytes);
                if (bytes < BLOCK_SIZE)
                    virtualDisk[temp->blockPointers[i]][bytes] = '\0';
                offset += bytes;
            }
            temp->size = dataSize;
            printf("Data written to '%s' (%d bytes).\n", fileName, dataSize);
            return;
        }
        temp = temp->next;
    }
    printf("Error: File '%s' not found.\n", fileName);
}

void readPerform(char *fileName) {
    if (fileName == NULL) {
        printf("Error: File name required.\n");
        return;
    }
    struct FileNode *temp = current->child;
    while (temp != NULL) {
        if (!temp->isDirectory && strcmp(temp->name, fileName) == 0) {
            if (temp->size == 0) {
                printf("File '%s' is empty.\n", fileName);
                return;
            }
            int remaining = temp->size;
            for (int i = 0; i < temp->numBlocks && remaining > 0; i++) {
                int bytes = (remaining > BLOCK_SIZE) ? BLOCK_SIZE : remaining;
                fwrite(virtualDisk[temp->blockPointers[i]], 1, bytes, stdout);
                remaining -= bytes;
            }
            printf("\n");
            return;
        }
        temp = temp->next;
    }
    printf("Error: File '%s' not found.\n", fileName);
}

void deletePerform(char *fileName) {
    if (fileName == NULL) {
        printf("Error: File name required.\n");
        return;
    }
    struct FileNode *prev = NULL, *temp = current->child;
    while (temp != NULL) {
        if (!temp->isDirectory && strcmp(temp->name, fileName) == 0) {
            for (int i = 0; i < temp->numBlocks; i++) {
                struct FreeBlockNode *blk = malloc(sizeof(struct FreeBlockNode));
                blk->index = temp->blockPointers[i];
                blk->next = NULL;
                blk->prev = blockListTail;
                if (blockListTail) blockListTail->next = blk;
                blockListTail = blk;
                if (!blockListHead) blockListHead = blk;
            }
            if (prev == NULL)
                current->child = temp->next;
            else
                prev->next = temp->next;

            free(temp);
            printf("File '%s' deleted successfully.\n", fileName);
            return;
        }
        prev = temp;
        temp = temp->next;
    }
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
            {
                char *fileName = strtok(NULL, " ");
                char *data = strtok(NULL, "");  
                writePerform(fileName, data);
                break;
            }
        }
        case 9: deletePerform(strtok(NULL, "")); break;
        case 10: cdPerform(strtok(NULL, "")); break;
        default: printf("Something went wrong.\n");
    }
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
    // Free memory before exit
    struct FreeBlockNode *temp = blockListHead;
    while (temp) {
        struct FreeBlockNode *next = temp->next;
        free(temp);
        temp = next;
    }
    free(root);

    return 0;
}