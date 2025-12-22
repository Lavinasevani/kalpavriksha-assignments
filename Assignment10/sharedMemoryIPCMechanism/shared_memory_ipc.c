/*
 * Shared Memory-Based IPC Mechanism
 * 
 * This program demonstrates interprocess communication using System V shared memory.
 * Parent Process: Reads array, places it in shared memory, displays before/after
 * Child Process: Sorts array in shared memory
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <semaphore.h>

/* ============= MACRO DEFINITIONS ============= */
#define MAX_ARRAY_SIZE 100
#define MAX_BUFFER_SIZE 1024
#define SHARED_MEMORY_KEY 7777
#define PARENT_PROCESS_LABEL "PARENT"
#define CHILD_PROCESS_LABEL "CHILD"
#define SUCCESS_EXIT_CODE 0
#define FAILURE_EXIT_CODE 1
#define SLEEP_TIME_SECONDS 2

/* ============= STRUCTURE DEFINITIONS ============= */
typedef struct {
    int array_size;
    int array_elements[MAX_ARRAY_SIZE];
    int sorting_complete_flag;
} SharedMemoryData;

/* ============= FUNCTION DECLARATIONS ============= */
void displayArray(int *array_pointer, int array_size, const char *stage_label);
void sortArrayAscending(int *array_pointer, int array_size);
void getUserInputArray(int *array_pointer, int *array_size_pointer);
void handleParentProcess(SharedMemoryData *shared_memory_pointer);
void handleChildProcess(SharedMemoryData *shared_memory_pointer);
int validateIntegerInput(const char *user_input_string);

/* ============= MAIN FUNCTION ============= */
int main()
{
    pid_t child_process_id;
    int shared_memory_id;
    SharedMemoryData *shared_memory_pointer;

    printf("\n=== Shared Memory-Based IPC Mechanism ===\n\n");

    /* Create shared memory segment */
    shared_memory_id = shmget(SHARED_MEMORY_KEY, sizeof(SharedMemoryData), 
                              IPC_CREAT | 0666);
    
    if (shared_memory_id == -1) {
        perror("Failed to create shared memory");
        exit(FAILURE_EXIT_CODE);
    }

    /* Attach shared memory */
    shared_memory_pointer = (SharedMemoryData *)shmat(shared_memory_id, NULL, 0);
    
    if (shared_memory_pointer == (SharedMemoryData *)-1) {
        perror("Failed to attach shared memory");
        shmctl(shared_memory_id, IPC_RMID, NULL);
        exit(FAILURE_EXIT_CODE);
    }

    /* Initialize shared memory */
    shared_memory_pointer->array_size = 0;
    shared_memory_pointer->sorting_complete_flag = 0;

    /* Fork child process */
    child_process_id = fork();

    if (child_process_id == -1) {
        perror("fork failed");
        shmdt(shared_memory_pointer);
        shmctl(shared_memory_id, IPC_RMID, NULL);
        exit(FAILURE_EXIT_CODE);
    }

    if (child_process_id == 0) {
        /* Child process */
        handleChildProcess(shared_memory_pointer);
        shmdt(shared_memory_pointer);
        exit(SUCCESS_EXIT_CODE);
    } else {
        /* Parent process */
        handleParentProcess(shared_memory_pointer);
        wait(NULL);

        /* Detach and remove shared memory */
        shmdt(shared_memory_pointer);
        shmctl(shared_memory_id, IPC_RMID, NULL);

        printf("\n[%s] Shared memory cleaned up\n", PARENT_PROCESS_LABEL);
    }

    return SUCCESS_EXIT_CODE;
}

/* ============= FUNCTION IMPLEMENTATIONS ============= */

/**
 * Displays array with descriptive label
 */
void displayArray(int *array_pointer, int array_size, const char *stage_label)
{
    printf("\n%s - Array Elements:\n", stage_label);
    printf("Size: %d\n", array_size);
    printf("Elements: ");

    for (int index = 0; index < array_size; index++) {
        printf("%d ", *(array_pointer + index));
    }
    printf("\n");
}

/**
 * Sorts array in ascending order using bubble sort
 */
void sortArrayAscending(int *array_pointer, int array_size)
{
    for (int outer_index = 0; outer_index < array_size - 1; outer_index++) {
        for (int inner_index = 0; inner_index < array_size - outer_index - 1; inner_index++) {
            if (*(array_pointer + inner_index) > *(array_pointer + inner_index + 1)) {
                int temporary_value = *(array_pointer + inner_index);
                *(array_pointer + inner_index) = *(array_pointer + inner_index + 1);
                *(array_pointer + inner_index + 1) = temporary_value;
            }
        }
    }
}

/**
 * Gets array input from user with validation
 */
void getUserInputArray(int *array_pointer, int *array_size_pointer)
{
    printf("\nEnter %d integers (separated by space or newline):\n", *array_size_pointer);

    for (int index = 0; index < *array_size_pointer; index++) {
        char element_input_buffer[MAX_BUFFER_SIZE];
        printf("Element %d: ", index + 1);

        if (fgets(element_input_buffer, sizeof(element_input_buffer), stdin) == NULL) {
            fprintf(stderr, "Error reading input\n");
            exit(FAILURE_EXIT_CODE);
        }

        if (!validateIntegerInput(element_input_buffer)) {
            fprintf(stderr, "Invalid input! Please enter a valid integer.\n");
            index--;
            continue;
        }

        *(array_pointer + index) = atoi(element_input_buffer);
    }
}

/**
 * Parent process handler
 */
void handleParentProcess(SharedMemoryData *shared_memory_pointer)
{
    int user_input_array[MAX_ARRAY_SIZE];
    char size_input_buffer[MAX_BUFFER_SIZE];

    printf("[%s] Started\n", PARENT_PROCESS_LABEL);

    /* Get input from user */
    printf("Enter number of integers to sort: ");

    if (fgets(size_input_buffer, sizeof(size_input_buffer), stdin) == NULL) {
        fprintf(stderr, "Error reading input\n");
        exit(FAILURE_EXIT_CODE);
    }

    if (!validateIntegerInput(size_input_buffer)) {
        fprintf(stderr, "Invalid input! Please enter a valid integer.\n");
        exit(FAILURE_EXIT_CODE);
    }

    int array_size = atoi(size_input_buffer);

    if (array_size <= 0 || array_size > MAX_ARRAY_SIZE) {
        fprintf(stderr, "Array size must be between 1 and %d\n", MAX_ARRAY_SIZE);
        exit(FAILURE_EXIT_CODE);
    }

    getUserInputArray(user_input_array, &array_size);

    /* Copy array to shared memory */
    printf("[%s] Copying array to shared memory...\n", PARENT_PROCESS_LABEL);
    shared_memory_pointer->array_size = array_size;

    for (int index = 0; index < array_size; index++) {
        *(shared_memory_pointer->array_elements + index) = user_input_array[index];
    }

    displayArray(shared_memory_pointer->array_elements, array_size, "[BEFORE SORTING]");

    /* Wait for child to sort */
    printf("\n[%s] Waiting for child process to sort...\n", PARENT_PROCESS_LABEL);
    
    while (shared_memory_pointer->sorting_complete_flag == 0) {
        sleep(1);
    }

    printf("[%s] Sorting completed by child process\n", PARENT_PROCESS_LABEL);

    /* Display sorted array */
    displayArray(shared_memory_pointer->array_elements, array_size, "[AFTER SORTING]");

    printf("\n[%s] Completed\n", PARENT_PROCESS_LABEL);
}

/**
 * Child process handler
 */
void handleChildProcess(SharedMemoryData *shared_memory_pointer)
{
    printf("[%s] Started and waiting for data...\n", CHILD_PROCESS_LABEL);

    /* Wait for parent to write data */
    while (shared_memory_pointer->array_size == 0) {
        sleep(1);
    }

    printf("[%s] Received array of size %d\n", CHILD_PROCESS_LABEL, 
           shared_memory_pointer->array_size);

    /* Sort the array in shared memory */
    sortArrayAscending(shared_memory_pointer->array_elements, 
                       shared_memory_pointer->array_size);

    printf("[%s] Array sorted in shared memory\n", CHILD_PROCESS_LABEL);

    /* Signal completion to parent */
    shared_memory_pointer->sorting_complete_flag = 1;

    printf("[%s] Completed\n", CHILD_PROCESS_LABEL);
}

/**
 * Validates if string contains valid integer
 */
int validateIntegerInput(const char *user_input_string)
{
    if (user_input_string == NULL || *user_input_string == '\0') {
        return 0;
    }

    int has_digit = 0;

    for (int index = 0; user_input_string[index] != '\0'; index++) {
        char current_character = user_input_string[index];

        if (current_character == '\n') {
            continue;
        }

        if (current_character >= '0' && current_character <= '9') {
            has_digit = 1;
        } else if (current_character == '-' && index == 0) {
            continue;
        } else {
            return 0;
        }
    }

    return has_digit;
}