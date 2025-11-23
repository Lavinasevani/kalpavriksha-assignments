#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define MIN_CAPACITY 1
#define MAX_CAPACITY 1000   
#define LARGEST_PRIME 1009
#define BUFFER 50

int TABLE_SIZE;
int capacity;
int currentSize = 0;

bool isPrime(int number){
    int smallestDivisor = 2;
    for(int divisor = smallestDivisor; divisor < sqrt(number) ; divisor++){
        if((number % divisor) == 0) return false;
    }
    return true;
}

int nextPrime(int number){
    for(int nextNumber = number + 1; nextNumber <= LARGEST_PRIME; nextNumber++){
        if(isPrime(nextNumber)){
            return nextNumber;
        }
    }
    return 0;
}

typedef struct QueueNode{
    int key;
    char *value;
    struct QueueNode *next;
    struct QueueNode *prev;
}QueueNode;

QueueNode *front = NULL; //LRU
QueueNode *rear = NULL;  //MRU

typedef struct HashNode{
    int key;
    QueueNode *nodeAddress;
    struct HashNode *next;
}HashNode;

HashNode **hashTable;

int createCache(){
    hashTable = (HashNode **)malloc(TABLE_SIZE * sizeof(HashNode *));
    if(hashTable == NULL){
        printf("Memory alloaction failed.");
        exit(EXIT_FAILURE);
    }
    for(int index = 0; index < TABLE_SIZE; index++){
        hashTable[index] = NULL;
    }
    return 0;
}

bool containsKey(int key){
    HashNode *currentNode = hashTable[key % TABLE_SIZE];
    while (currentNode  != NULL)
    {
        if(currentNode ->key == key){
            return true;
        }
        currentNode  = currentNode ->next;
    }
    return false;
}

bool isCacheFull(){
    return capacity == currentSize;
}

void performPUT(int key, char *value){

    if(containsKey(key)){ // Update the existing key's value
        int index = key % TABLE_SIZE;
        HashNode *currentNode = hashTable[index];
        while (currentNode != NULL)
        {
            if(currentNode->key == key){

                QueueNode *temp = currentNode->nodeAddress;

                free(temp->value);
                temp->value = malloc(strlen(value) + 1);
                strcpy(temp->value, value);
                
                if(temp != rear){
                    if (temp == front) {
                        front = front->next;
                        if (front) front->prev = NULL;
                    }
                    else {
                        temp->prev->next = temp->next;
                        temp->next->prev = temp->prev;
                    }
                    temp->prev = rear;
                    temp->next = NULL;
                    rear->next = temp;
                    rear = temp;
                }
                printf("Key %d already exists so value updated successfully.\n", rear->key);
                return;
            }
            currentNode = currentNode->next;
        }
    }
    if(isCacheFull()){ // evict the LRU 
        printf("Cache is full. => ");
        QueueNode *oldFront = front;
        int evictedKey = front->key;
        front = front->next;
        if (front != NULL) {
            front->prev = NULL;
        } else {
            rear = NULL; 
        }

        HashNode *curr = hashTable[evictedKey % TABLE_SIZE];
        HashNode *prev = NULL;
        printf("Eviction Begin... => ");
        while (curr != NULL) {
            if (curr->key == evictedKey) {
                if(prev == NULL){
                    hashTable[evictedKey % TABLE_SIZE] = curr->next; 
                }else {
                    prev->next = curr->next;
                }
                free(curr);
                break;
            }
            prev = curr;
            curr = curr->next;
        }

        free(oldFront->value);
        free(oldFront);
        printf("Node with key ( %d ) LRU evicted success\n", evictedKey);
        currentSize--;

    }
    // add a new process at MRU 
   QueueNode *node = (QueueNode *)malloc(sizeof(QueueNode));
    node->key = key;
    node->value = malloc(strlen(value) + 1);
    strcpy(node->value, value);
    node->next = NULL;
    node->prev = rear;

    if(front == NULL && rear == NULL){
        front = node;
        rear = node;
    }else{
        rear->next = node;
        rear = node;
    }
    int index = key % TABLE_SIZE;
    HashNode *newhNode = malloc(sizeof(HashNode));
    newhNode->key = key;
    newhNode->nodeAddress = node;
    newhNode->next = hashTable[index];
    hashTable[index] = newhNode;
    currentSize++;
    return;
    
}

char* performGET(int key){
    if(!containsKey(key)){
        return "NULL";
    }
    int index = key % TABLE_SIZE;
    HashNode *hNode = hashTable[index];
    while (hNode != NULL)
    {
        if(hNode->key  == key){
            QueueNode *targetNode = hNode->nodeAddress;
            // update MRU
            if(targetNode == rear) return targetNode->value;

            if(targetNode == front){
                front = front->next;
                front->prev = NULL;
            } else {
                targetNode->prev->next = targetNode->next;
                targetNode->next->prev = targetNode->prev;
            }
            targetNode->prev = rear;
            targetNode->next = NULL;
            rear->next = targetNode;
            rear = targetNode;
            return targetNode->value;
        }
        hNode = hNode->next;
    }
    return "NULL";
}

void freeCache(){
    QueueNode *temp = front;
    while (temp != NULL)
    {
        QueueNode *nextNode = temp->next;
        free(temp->value);
        free(temp);
        temp = nextNode;
    }

    for(int index = 0; index < TABLE_SIZE; index++){
        HashNode *tempHash = hashTable[index];
        while (tempHash != NULL)
        {
            HashNode *next = tempHash;
            free(tempHash);
            tempHash = next;
        }   
    }
    free(hashTable);
    return;
}

int main(){
    printf("Create Cache : ");
    if(scanf("%d", &capacity) != 1){
        printf("Invalid number.");
        while (getchar() != '\n');
        return 1;
    }
    getchar();
    if(capacity < MIN_CAPACITY || capacity > MAX_CAPACITY){
        printf("Capacity of cache should between  %d - %d.", MIN_CAPACITY, MAX_CAPACITY);
        return 1;
    }

    TABLE_SIZE = nextPrime(capacity); 

    createCache();

    const char *validCommands[] = {"get", "put", "exit"};

    char *input = (char *)malloc(BUFFER * sizeof(char));
    if(input == NULL){
        printf("Memory alloaction failed !!");
        return 1;
    }

    while(true){
         if(fgets(input, BUFFER, stdin) == NULL){
            printf("Something went wrong");
            exit(EXIT_FAILURE); 
        }
        if(input[0] == '\n' || input[0] == '\0'){
            printf("You can enter command (\"get\", \"put\", \"exit\"). \n");
            continue;
        }
        input[strcspn(input, "\n")] = '\0';

        char *command= strtok(input, " ");

        if(strcmp(command, validCommands[0]) == 0){ //GET
            char *k = strtok(NULL, " ");
            if(k == NULL){
                printf("Key is empty\n");
                continue;
            }
            char *endptr;
            int key = strtol(k, &endptr, 10);
            if (*endptr != '\0') {
                printf("Invalid key: %s\n", k);
                continue;
            }
            else{
                int key = atoi(k);
                printf("%s\n", performGET(key));
            }
        }
        else if(strcmp(command, validCommands[1]) == 0){ //PUT
            char *k = strtok(NULL, " ");
            if(k == NULL){
                printf("Key is empty\n");
                continue;
            }
            char *endptr;
            int key = strtol(k, &endptr, 10);
            if (*endptr != '\0') {
                printf("Invalid key: %s\n", k);
                continue;
            }
            else{
                char *token = strtok(NULL, "");
                if(token == NULL){
                    printf("Value is empty\n");
                    continue;
                }
                int key = atoi(k);
                performPUT(key, token);
            }
        }
        else if(strcmp(command, validCommands[2]) == 0){
            free(input);
            freeCache();
            exit(EXIT_SUCCESS); 
        }
        else{
            printf("Invalid Command !! \nTry Again!!\n");
        }
    }
    return 0;
}