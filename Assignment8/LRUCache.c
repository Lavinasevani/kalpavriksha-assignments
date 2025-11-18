#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define MIN_CAPACITY 1
#define MAX_CAPACITY 1000   

int TABLE_SIZE;

bool isPrime(int number){
    int divisor = 2;
    for(int i = divisor; i < sqrt(number) ; i++){
        if((number % i) == 0) return false;
    }
    return true;
}

int nextPrime(int number){
    for(int i = number+1; i <= 1009; i++){
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
    int capacity;
    if(scanf("%d", &capacity) != 1){
        printf("Invalid number.");
        while (getchar() != '\n');
        return 1;
    }
    if(capacity < MIN_CAPACITY || capacity > MAX_CAPACITY){
        printf("Capacity of cache should between  %d - %d.", MIN_CAPACITY, MAX_CAPACITY);
        return 1;
    }
    TABLE_SIZE = nextPrime(capacity); 
    printf("Capacity = %d \nTable Size = %d", capacity, TABLE_SIZE);
    createCache();
    return 0;
}