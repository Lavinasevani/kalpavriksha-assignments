#include <stdio.h>
#include <stdlib.h> // exit()
#include <unistd.h> // fork(), pipe(), read(), write()
#include <sys/wait.h> // wait()

#define MAX_ARRAY_SIZE 100

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
        printf("Size of array should be less then or equals to %d.\n", MAX_ARRAY_SIZE);
        exit(1);
    }

    printf("Enter %d elements:\n",arraySize);
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
    int dataArray[MAX_ARRAY_SIZE];
    int arraySize;
    int pipeParentToChild[2];
    int pipeChildToParent[2];
    pid_t pid;

    arraySize = readArray(dataArray);

    displayArray("Before Sorting:", dataArray, arraySize);

    pipe(pipeParentToChild);
    pipe(pipeChildToParent);

    pid = fork();

    if (pid < 0) {
        printf("Fork failed\n");
        exit(1);
    }

    if (pid == 0) {
        close(pipeParentToChild[1]); 
        close(pipeChildToParent[0]); 

        read(pipeParentToChild[0], &arraySize, sizeof(int));
        read(pipeParentToChild[0], dataArray, sizeof(int) * arraySize);

        sortArray(dataArray, arraySize);

        write(pipeChildToParent[1], &arraySize, sizeof(int));
        write(pipeChildToParent[1], dataArray, sizeof(int) * arraySize);

        close(pipeParentToChild[0]);
        close(pipeChildToParent[1]);

        exit(0);
    } 
    else {
        close(pipeParentToChild[0]); //read
        close(pipeChildToParent[1]); //write

        write(pipeParentToChild[1], &arraySize, sizeof(int));
        write(pipeParentToChild[1], dataArray, sizeof(int) * arraySize);

        wait(NULL); 

        read(pipeChildToParent[0], &arraySize, sizeof(int));
        read(pipeChildToParent[0], dataArray, sizeof(int) * arraySize);

        displayArray("After Sorting:", dataArray, arraySize);

        close(pipeParentToChild[1]);
        close(pipeChildToParent[0]);
    }

    return 0;
}
