#include <stdio.h>
#include <stdlib.h> //exit()
#include <unistd.h> //fork()
#include <sys/wait.h> //wait()
#include <sys/ipc.h> //IPC_CREAT, IPC_RMID
#include <sys/shm.h> //shmget()  semgat() shmdt shmctl()

#define MAX_ARRAY_SIZE 100
#define SHARED_MEMORY_KEY 1234

struct sharedData {
    int arraySize;
    int dataArray[MAX_ARRAY_SIZE];
};

int readArray(int dataArray[]) {
    int arraySize;

    printf("Enter number of elements: ");
    if (scanf("%d", &arraySize) != 1) {
        printf("Array Size should be the number only.\n");
        exit(1);
    }
    if (arraySize <= 0) {
        printf("Size of array can't be negative.\n");
        exit(1);
    }
    if (arraySize > MAX_ARRAY_SIZE) {
        printf("Size of array should be less than or equal to %d.\n", MAX_ARRAY_SIZE);
        exit(1);
    }

    printf("Enter %d elements:\n", arraySize);
    for (int index = 0; index < arraySize; index++) {
        if (scanf("%d", &dataArray[index]) != 1) {
            printf("Array element should be number only.\n");
            exit(1);
        }
    }

    return arraySize;
}

void sortArray(int dataArray[], int arraySize) {
    for (int i = 0; i < arraySize - 1; i++) {
        for (int j = 0; j < arraySize - i - 1; j++) {
            if (dataArray[j] > dataArray[j + 1]) {
                int temp = dataArray[j];
                dataArray[j] = dataArray[j + 1];
                dataArray[j + 1] = temp;
            }
        }
    }
}

void displayArray(const char *message, int dataArray[], int arraySize) {
    printf("\n%s ", message);
    for (int i = 0; i < arraySize; i++) {
        printf("%d ", dataArray[i]);
    }
    printf("\n");
}

int main() {
    int sharedMemoryId;
    struct sharedData *sharedMemory;
    pid_t pid;

    sharedMemoryId = shmget(SHARED_MEMORY_KEY, sizeof(struct sharedData), IPC_CREAT | 0666); //create shared memory
    if (sharedMemoryId < 0) {
        perror("shmget failed");
        exit(1);
    }

    sharedMemory = (struct sharedData *)shmat(sharedMemoryId, NULL, 0); //attach memory
    if (sharedMemory == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }

    sharedMemory->arraySize = readArray(sharedMemory->dataArray);
    displayArray("Before Sorting:", sharedMemory->dataArray, sharedMemory->arraySize);

    pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {//child process
        sortArray(sharedMemory->dataArray, sharedMemory->arraySize);
        shmdt(sharedMemory); //deattach memory
        exit(0);
    } 
    else {
        wait(NULL);

        displayArray("After Sorting:", sharedMemory->dataArray, sharedMemory->arraySize);

        shmdt(sharedMemory); //deattach memory
        shmctl(sharedMemoryId, IPC_RMID, NULL); //removes shared memory segment
    }

    return 0;
}
