#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_STRING_LENGTH 50
#define BLOCK_SIZE 512
#define NUM_OF_BLOCKS 1024

struct FileNode{
    char name[MAX_STRING_LENGTH];
    bool isDirectory; //directory or file 
    struct FileNode *parent; // parent 
    struct FileNode *child; // child 
    struct FileNode *next; // sibling
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
        newBlock->prev = NULL;
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
void mkdirPerform(char *directoryName){
    
    struct FileNode *temp = current->child;
    while (temp != NULL) {
        if (strcmp(temp->name, directoryName) == 0 && temp->isDirectory) {
            printf("Error: Directory '%s' already exists.\n", directoryName);
            return;
        }
        temp = temp->next;
    }

    // remaining name validations :  
    //A file name can't contain any of the following characters: \ / * ? < > : " |
    //len can't be 0 means can't be empty , NULL , or caintain soace only 

    struct FileNode *newDirectory = (struct FileNode*)malloc(sizeof(struct FileNode));
    if(newDirectory == NULL){
        printf("Memory alloaction fail");
        exit(1);
    }

    strcpy(newDirectory->name, directoryName);
    newDirectory->isDirectory = true;
    newDirectory->next = root;
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
        printf("You can delete the root directory");
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
    if (current == NULL) {
        printf("Error: No current directory.\n");
        return;
    }
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

void execute(char *input){
    if(input == NULL) return;
    char *inputCommand = strtok(input," ");
    int cmdIndex = getCommandIndex(inputCommand);
    if (cmdIndex == -1) {
        printf("Invalid Command");
        return;
    }
    switch (cmdIndex)
    {
        case 0: {
            char *directoryName= strtok(NULL, " "); // NULL because same old one string the alredy been processed
            mkdirPerform(directoryName); 
            break;
        }
        case 1:{
            char *directoryNameToDelete= strtok(NULL, " "); // NULL because same old one string the alredy been processed
            rmdirPerform(directoryNameToDelete); 
            break;
        }
        case 2: 
            pwdPerform(); 
            break;
        case 3:
            cwdPerform(); 
            break;
        case 4: 
            dfPerform(); 
            break;
        case 5: 
            lsPerform(); 
            break;
        case 6: 
            printf("create");   //createPerform(); 
            break;
        case 7: 
            printf("read"); //readPerform(); 
            break;
        case 8: 
            printf("write");     //writePerform(); 
            break;
        case 9: 
            printf("delete");   //deletePerform(); 
            break;
        case 10:{
            char *argument= strtok(NULL, " "); 
            cdPerform(argument); 
            break;
        }
        default: 
            printf("Command not implemented yet.\n");
    }
}    

int main() {
    printf("$ ./vfs\nCompact VFS - ready. Type 'exit' to quit.");

    initializeRootDirectory();
    initializeFreeBlocks();
    char *input= malloc(MAX_STRING_LENGTH * sizeof(char));
    if(input== NULL){
        printf("Memory allocation failed");
        return 1;
    }
    while (1) {
        printPrompt();
        if (fgets(input, MAX_STRING_LENGTH, stdin) == NULL) break;
        input[strcspn(input, "\n")] = '\0';
        if (strlen(input) == 0)
            continue;
        if (strcmp(input, "exit") == 0)
            break;
        execute(input);
    }
    free(input);
    free(root);
    struct FreeBlockNode *temp = blockListHead;
    while(temp){
        struct FreeBlockNode *next = temp->next;
        free(temp);
        temp = next;
    }

    return 0;
}