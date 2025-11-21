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
int currSize = 0;

bool isPrime(int number){
    int divisor = 2;
    for(int i = divisor; i < sqrt(number) ; i++){
        if((number % i) == 0) return false;
    }
    return true;
}

int nextPrime(int number){
    for(int i = number+1; i <= LARGEST_PRIME; i++){
        if(isPrime(i)){
            return i;
        }
    }
    return 0;
}

typedef struct Queue{
    int key;
    char *value;
    struct Queue *next;
    struct Queue *prev;
}Queue;

Queue *front = NULL;
Queue *rear = NULL;

typedef struct HashMap{
    int key;
    Queue *queuePointer;
    struct HashMap *next;
}HashMap;

//hash function (Division Method) -> decide the index of key 
HashMap **bucket;

int createCache(){
    bucket = (HashMap **)malloc(TABLE_SIZE * sizeof(HashMap *));
    if(bucket == NULL){
        printf("Memory alloaction failed.");
        exit(EXIT_FAILURE);
    }
    for(int index = 0; index < TABLE_SIZE; index++){
        bucket[index] = NULL;
    }
    return 0;
}

bool containsKey(int key){
    HashMap *curr = bucket[key % TABLE_SIZE];
    while (curr != NULL)
    {
        if(curr->key == key){
            return true;
        }
        curr = curr->next;
    }
    return false;
}


bool isFull(){
    return capacity == currSize;
}

char* performGET(int key){
    return "NULL";
}

void performPUT(int key, char *value){

    if(containsKey(key)){ // Update the existing key's value
        int index = key % TABLE_SIZE;
        HashMap *curr = bucket[index];
        while (curr != NULL)
        {
            if(curr->key == key){

                Queue *temp = curr->queuePointer;

                free(temp->value);
                temp->value = malloc(strlen(value) + 1);
                strcpy(temp->value, value);
                if (temp == rear) return;
                
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
                printf("Key already exists so value updated successfully.\n");
                printf("Key: %d , Updated Value: %s \n", rear->key, rear->value);
                return;
            }
            curr = curr->next;
        }
    }
    if(isFull()){ // evict the LRU 
        printf("Cache is full. => ");
        Queue *temp = front;
        int evictedKey = front->key;
        front = front->next;
        if (front != NULL) {
            front->prev = NULL;
        } else {
            rear = NULL; 
        }

        HashMap *curr = bucket[evictedKey % TABLE_SIZE];
        HashMap *prev = NULL;
        printf("Eviction Begin... => ");
        while (curr != NULL) {
            if (curr->key == evictedKey) {
                if(prev == NULL){
                    bucket[evictedKey % TABLE_SIZE] = curr->next; 
                }else {
                    prev->next = curr->next;
                }
                free(curr);
                break;
            }
            prev = curr;
            curr = curr->next;
        }

        free(temp->value);
        free(temp);
        printf("Node with key evicted : %d\n", evictedKey);
        currSize--;

    }
    // add a new process at MRU 
    Queue *node = (Queue *)malloc(sizeof(Queue));
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
    HashMap *hNode = malloc(sizeof(HashMap));
    hNode->key = key;
    hNode->queuePointer = node;
    hNode->next = bucket[index];
    bucket[index] = hNode;
    currSize++;
    printf("Put key performed successfully\n");
    printf("MRU key : %d , Value: %s\n", rear->key, rear->value);
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
                printf("Command : %s \t", command);
                int key = atoi(k);
                printf("Key : %d\n", key);
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
                char *token = strtok(NULL, " ");
                if(token == NULL){
                    printf("Value is empty\n");
                    continue;
                }
                printf("Command : %s \t", command);
                int key = atoi(k);
                printf("Key : %d\t", key);
                printf("Value: %s\n", token);
                performPUT(key, token);
            }
        }
        else if(strcmp(command, validCommands[2]) == 0){
            exit(EXIT_SUCCESS); 
        }
        else{
            printf("Invalid Command !! \nTry Again!!\n");
        }
    }
    return 0;
}