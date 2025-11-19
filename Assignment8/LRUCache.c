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
int createCache(){
    HashMap *hashMapArr[TABLE_SIZE];
    return 0;
}

int main(){
    printf("Create Cache : ");
    int capacity;
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
                char *tokenNumber = strtok(NULL, " ");
                if(tokenNumber == NULL){
                    printf("Value is empty\n");
                    continue;
                }
                printf("Command : %s \t", command);
                int key = atoi(k);
                printf("Key : %d\t", key);
                printf("Value: %s\n", tokenNumber);
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