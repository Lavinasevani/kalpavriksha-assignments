#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>

#define MAX_ARRAY_SIZE 100
#define MAX_BUFFER_SIZE 1024
#define SHARED_MEMORY_KEY 7777

typedef struct {
    int array_size;
    int array_elements[MAX_ARRAY_SIZE];
} SharedMemoryData;

int validateIntegerInput(const char *input)
{
    int i = 0;
    if (input[0] == '-') i++;

    for (; input[i] != '\n' && input[i] != '\0'; i++) {
        if (input[i] < '0' || input[i] > '9')
            return 0;
    }
    return 1;
}

void displayArray(int *array, int size, const char *label)
{
    printf("\n%s\nArray Size: %d\nElements: ", label, size);
    for (int i = 0; i < size; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
}

void sortArrayAscending(int *array, int size)
{
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (array[j] > array[j + 1]) {
                int temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}

void getUserInputArray(int *array, int size)
{
    char buffer[MAX_BUFFER_SIZE];
    for (int i = 0; i < size; i++) {
        printf("Element %d: ", i + 1);
        fgets(buffer, sizeof(buffer), stdin);

        if (!validateIntegerInput(buffer)) {
            printf("Invalid input, try again\n");
            i--;
            continue;
        }
        array[i] = atoi(buffer);
    }
}

int main()
{
    int shm_id;
    SharedMemoryData *shm_ptr;
    pid_t pid;

    printf("\n=== Shared Memory-Based IPC Mechanism ===\n\n");

    // Create shared memory 
    shm_id = shmget(SHARED_MEMORY_KEY, sizeof(SharedMemoryData),
                    IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget failed");
        exit(1);
    }

    // Attach shared memory
    shm_ptr = (SharedMemoryData *)shmat(shm_id, NULL, 0);
    if (shm_ptr == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }

    // Parent takes input FIRST 
    char buffer[MAX_BUFFER_SIZE];
    printf("Enter number of integers to sort: ");
    fgets(buffer, sizeof(buffer), stdin);

    if (!validateIntegerInput(buffer)) {
        printf("Invalid input\n");
        exit(1);
    }

    shm_ptr->array_size = atoi(buffer);

    if (shm_ptr->array_size <= 0 || shm_ptr->array_size > MAX_ARRAY_SIZE) {
        printf("Invalid array size\n");
        exit(1);
    }

    getUserInputArray(shm_ptr->array_elements, shm_ptr->array_size);

    displayArray(shm_ptr->array_elements,
                 shm_ptr->array_size,
                 "[BEFORE SORTING]");

    // Fork AFTER data is ready 
    pid = fork();

    if (pid == -1) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {
        // CHILD PROCESS 
        sortArrayAscending(shm_ptr->array_elements, shm_ptr->array_size);
        exit(0);
    } else {
        // PARENT PROCESS 
        wait(NULL);  

        displayArray(shm_ptr->array_elements,
                     shm_ptr->array_size,
                     "[AFTER SORTING]");

        shmdt(shm_ptr);
        shmctl(shm_id, IPC_RMID, NULL);
        printf("\n[PARENT] Shared memory cleaned up\n");
    }

    return 0;
}