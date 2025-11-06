#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_STRING_LENGTH 50
#define BLOCK_SIZE 512
#define NUM_OF_BLOCKS 1024

struct FileNode{
    char name[MAX_STRING_LENGTH];
    bool isDirectory;
    struct FileNode *parent;
    struct FileNode *child;
    struct FileNode *next;
};

char virtualDisk[BLOCK_SIZE][NUM_OF_BLOCKS];

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
    root->next = root;
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
            blockListTail = newBlock;
        }else{
            blockListTail->next = newBlock;
            newBlock->prev = blockListTail;
            blockListTail = newBlock;
        }
    }
    return;
}

void currentDirectory(){
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
        case 0: 
            printf("mkdir"); // mkdirPerform(); 
            break;
        case 1: 
            printf("rmdir"); // rmdirPerform(); 
            break;
        case 2: 
            printf("pwd"); // pwdPerform(); 
            break;
        case 3:
            printf("cwd"); //cwdPerform(); 
            break;
        case 4: 
            printf("df"); //dfPerform(); 
            break;
        case 5: 
            printf("ls");    //lsPerform(); 
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
        case 10: 
            printf("cd");   //cdPerform(); 
            break;
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
        currentDirectory();
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