#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_ARRAY_SIZE 100
#define MESSAGE_KEY 1234

struct message {
    long messageType;
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
        printf("Size of array should be less then or equals to %d.\n", MAX_ARRAY_SIZE);
        exit(1);
    }

    printf("Enter %d elements:\n", arraySize);
    for (int i = 0; i < arraySize; i++) {
        if (scanf("%d", &dataArray[i]) != 1) {
            printf("Invalid element\n");
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
    int messageQueueId;
    pid_t pid;
    struct message msg;

    messageQueueId = msgget(MESSAGE_KEY, IPC_CREAT | 0666); //create message queue
    if (messageQueueId == -1) {
        printf("msgget failed");
        exit(1);
    }

    pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {

        msgrcv(messageQueueId, &msg, sizeof(msg) - sizeof(long), 1, 0); // child recive array from parent

        sortArray(msg.dataArray, msg.arraySize);

        msg.messageType = 2;
        msgsnd(messageQueueId, &msg, sizeof(msg) - sizeof(long), 0); //child sends sorted array back

        exit(0);
    } 
    else {

        msg.arraySize = readArray(msg.dataArray);
        displayArray("Before Sorting:", msg.dataArray, msg.arraySize);

        msg.messageType = 1; // Type 1 data for child
        msgsnd(messageQueueId, &msg, sizeof(msg) - sizeof(long), 0); // Sends array size and array

        wait(NULL);

        msgrcv(messageQueueId, &msg, sizeof(msg) - sizeof(long), 2, 0); //recive sorted array from child 
        displayArray("After Sorting:", msg.dataArray, msg.arraySize);

        msgctl(messageQueueId, IPC_RMID, NULL); //remove message queue from kernel
    }

    return 0;
}
