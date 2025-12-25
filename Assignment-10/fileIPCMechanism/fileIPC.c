#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARRAY_SIZE 100
#define FILE_NAME "ipc_data.txt"


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

void displayArray(const char *message, int dataArray[], int arraySize) {
    int index;
    printf("\n%s ", message);
    for (index = 0; index < arraySize; index++) {
        printf("%d ", dataArray[index]);
    }
    printf("\n");
}

void writeArrayToFile(int dataArray[], int arraySize) {

    FILE *filePointer = fopen(FILE_NAME, "w");

    if (filePointer == NULL) {
        printf("File open failed");
        exit(1);
    }

    fprintf(filePointer, "%d\n", arraySize);

    for (int index = 0; index < arraySize; index++) {
        fprintf(filePointer, "%d ", dataArray[index]);
    }

    fclose(filePointer);
}

void readArrayFromFile(int dataArray[], int *arraySize) {

    FILE *filePointer = fopen(FILE_NAME, "r");

    if (filePointer == NULL) {
        printf("File open failed");
        exit(1);
    }

    fscanf(filePointer, "%d", arraySize);

    for (int index = 0; index < *arraySize; index++) {
        fscanf(filePointer, "%d", &dataArray[index]);
    }

    fclose(filePointer);
}

void sortArray(int dataArray[], int arraySize) {

    for (int i = 0; i < arraySize - 1; i++) {
        for (int j = 0; j < arraySize - i- 1; j++) {
            if (dataArray[j] > dataArray[j + 1]) {
                int temp = dataArray[j];
                dataArray[j] = dataArray[j + 1];
                dataArray[j + 1] = temp;
            }
        }
    }

}


int main() {

    int dataArray[MAX_ARRAY_SIZE];
    int arraySize;
    pid_t pid;

    arraySize = readArray(dataArray);

    displayArray("Before Sorting:", dataArray, arraySize);

    writeArrayToFile(dataArray, arraySize);

    pid = fork();

    if (pid < 0) {
        printf("Fork failed");
        exit(1);
    }

    if (pid == 0) {
        readArrayFromFile(dataArray, &arraySize);
        sortArray(dataArray, arraySize);
        writeArrayToFile(dataArray, arraySize);
        exit(0);
    }

    wait(NULL);

    readArrayFromFile(dataArray, &arraySize);
    displayArray("After Sorting:", dataArray, arraySize);

    return 0;
}
