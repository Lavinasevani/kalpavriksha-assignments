#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define DATA_FILE "ipc_data.txt"
// #define LOCK_FILE "ipc_lock.txt"
#define MAX_ARRAY_SIZE 100
#define MAX_BUFFER_SIZE 1024
#define SLEEP_INTERVAL 1
#define PROCESS_1_LABEL "PROCESS_1"
#define PROCESS_2_LABEL "PROCESS_2"

/* ============= FUNCTION DECLARATIONS ============= */
void displayArray(int *array_pointer, int array_size, const char *stage_label);
void readArrayFromFile(int *array_pointer, int *array_size_pointer);
void writeArrayToFile(int *array_pointer, int array_size);
void sortArrayAscending(int *array_pointer, int array_size);
void getUserInputArray(int *array_pointer, int *array_size_pointer);
void handleValidationAndSort();
void handleDisplayProcess();
int validateIntegerInput(const char *user_input_string);

/* ============= MAIN FUNCTION ============= */
int main(int argument_count, char *argument_vector[])
{
    pid_t child_process_id;
    int argument_index;
    int user_input_array[MAX_ARRAY_SIZE];
    int actual_array_size = 0;

    if (argument_count < 2) {
        printf("\n=== File-Based IPC Mechanism ===\n");
        printf("Usage: %s <option>\n", argument_vector[0]);
        printf("Options:\n");
        printf("  1 - Display process (reads and displays data)\n");
        printf("  2 - Validation and sort process\n");
        printf("\nRunning complete demo...\n\n");

        /* Take input from user */
        printf("Enter number of integers to sort: ");
        char size_input_buffer[MAX_BUFFER_SIZE];
        if (fgets(size_input_buffer, sizeof(size_input_buffer), stdin) == NULL) {
            fprintf(stderr, "Error reading input\n");
            exit(EXIT_FAILURE);
        }

        if (!validateIntegerInput(size_input_buffer)) {
            fprintf(stderr, "Invalid input! Please enter a valid integer.\n");
            exit(EXIT_FAILURE);
        }

        actual_array_size = atoi(size_input_buffer);

        if (actual_array_size <= 0 || actual_array_size > MAX_ARRAY_SIZE) {
            fprintf(stderr, "Array size must be between 1 and %d\n", MAX_ARRAY_SIZE);
            exit(EXIT_FAILURE);
        }

        getUserInputArray(user_input_array, &actual_array_size);
        writeArrayToFile(user_input_array, actual_array_size);

        /* Create child process for sorting */
        child_process_id = fork();

        if (child_process_id == -1) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }

        if (child_process_id == 0) {
            /* Child process: Sort the data */
            handleValidationAndSort();
            exit(EXIT_SUCCESS);
        } else {
            /* Parent process: Display before and after */
            handleDisplayProcess();
            wait(NULL);
        }

        return EXIT_SUCCESS;
    }

    /* Handle specific process argument */
    argument_index = atoi(argument_vector[1]);

    if (argument_index == 1) {
        handleDisplayProcess();
    } else if (argument_index == 2) {
        handleValidationAndSort();
    } else {
        fprintf(stderr, "Invalid argument. Use 1 or 2\n");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
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
 * Reads array from file
 */
void readArrayFromFile(int *array_pointer, int *array_size_pointer)
{
    FILE *file_pointer = fopen(DATA_FILE, "r");

    if (file_pointer == NULL) {
        perror("Cannot open data file for reading");
        exit(EXIT_FAILURE);
    }

    if (fscanf(file_pointer, "%d", array_size_pointer) != 1) {
        fprintf(stderr, "Error reading array size from file\n");
        fclose(file_pointer);
        exit(EXIT_FAILURE);
    }

    if (*array_size_pointer > MAX_ARRAY_SIZE || *array_size_pointer <= 0) {
        fprintf(stderr, "Invalid array size in file: %d\n", *array_size_pointer);
        fclose(file_pointer);
        exit(EXIT_FAILURE);
    }

    for (int index = 0; index < *array_size_pointer; index++) {
        if (fscanf(file_pointer, "%d", (array_pointer + index)) != 1) {
            fprintf(stderr, "Error reading element %d from file\n", index);
            fclose(file_pointer);
            exit(EXIT_FAILURE);
        }
    }

    fclose(file_pointer);
}

/**
 * Writes array to file
 */
void writeArrayToFile(int *array_pointer, int array_size)
{
    FILE *file_pointer = fopen(DATA_FILE, "w");

    if (file_pointer == NULL) {
        perror("Cannot open data file for writing");
        exit(EXIT_FAILURE);
    }

    fprintf(file_pointer, "%d\n", array_size);

    for (int index = 0; index < array_size; index++) {
        fprintf(file_pointer, "%d ", *(array_pointer + index));
    }
    fprintf(file_pointer, "\n");

    fclose(file_pointer);
}

/**
 * Sorts array in ascending order
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
 * Gets array input from user
 */
void getUserInputArray(int *array_pointer, int *array_size_pointer)
{
    printf("\nEnter %d integers (separated by space or newline):\n", *array_size_pointer);

    for (int index = 0; index < *array_size_pointer; index++) {
        char element_input_buffer[MAX_BUFFER_SIZE];
        printf("Element %d: ", index + 1);

        if (fgets(element_input_buffer, sizeof(element_input_buffer), stdin) == NULL) {
            fprintf(stderr, "Error reading input\n");
            exit(EXIT_FAILURE);
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
 * Validation and sort process
 */
void handleValidationAndSort()
{
    int data_array[MAX_ARRAY_SIZE];
    int actual_array_size = 0;

    sleep(SLEEP_INTERVAL);

    readArrayFromFile(data_array, &actual_array_size);
    sortArrayAscending(data_array, actual_array_size);
    writeArrayToFile(data_array, actual_array_size);

    printf("\n[%s] Data sorted and written to file\n", PROCESS_2_LABEL);
}

/**
 * Display process - shows before and after
 */
void handleDisplayProcess()
{
    int data_array[MAX_ARRAY_SIZE];
    int actual_array_size = 0;

    readArrayFromFile(data_array, &actual_array_size);
    displayArray(data_array, actual_array_size, "[BEFORE SORTING]");

    sleep(SLEEP_INTERVAL * 3);

    readArrayFromFile(data_array, &actual_array_size);
    displayArray(data_array, actual_array_size, "[AFTER SORTING]");
}

/**
 * Validates if string contains valid integer
 */
int validateIntegerInput(const char *user_input_string)
{
    if (user_input_string == NULL || *user_input_string == '\0') {
        return 0;
    }

    for (int index = 0; user_input_string[index] != '\0'; index++) {
        if (user_input_string[index] == '\n') {
            continue;
        }
        if (user_input_string[index] < '0' || user_input_string[index] > '9') {
            if (index == 0 && user_input_string[index] != '-') {
                return 0;
            }
            if (index > 0) {
                return 0;
            }
        }
    }

    return 1;
}