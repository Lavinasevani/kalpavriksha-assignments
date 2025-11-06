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
struct FreeBlockNode *blockHead = NULL;
struct FreeBlockNode *blockTail = NULL;

void initalizeRootDirectory(){
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

void currentDirectory(){
    printf("\n%s > ", current->name);
    return;
}

int main() {
    printf("$ ./vfs\nCompact VFS - ready. Type 'exit' to quit.");

    initalizeRootDirectory();

    char *choice = malloc(MAX_STRING_LENGTH * sizeof(char));
    while (1) {
        currentDirectory();
        if (fgets(choice, 50, stdin) == NULL) break;
        choice[strcspn(choice, "\n")] = '\0';
        if (strlen(choice) == 0)
            continue;
        if (strcmp(choice, "exit") == 0)
            break;
        // performOperation(choice);
        printf("You entered: %s", choice);
    }
    free(choice);
    free(root);
    return 0;
}