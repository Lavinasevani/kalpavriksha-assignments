#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define DATA_FILE "ipc_data.txt"
#define MAX_ARRAY_SIZE 100
#define MAX_BUFFER_SIZE 1024
#define PROCESS_1_LABEL "PROCESS_1"
#define PROCESS_2_LABEL "PROCESS_2"

int validateIntegerInput(const char *str)
{
    if (!str || *str == '\0') return 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') continue;
        if ((str[i] < '0' || str[i] > '9') && !(i == 0 && str[i] == '-')) return 0;
    }
    return 1;
}

void displayArray(int *array_pointer, int array_size, const char *stage_label)
{
    printf("\n%s - Array Elements:\n", stage_label);
    printf("Size: %d\n", array_size);
    printf("Elements: ");
    for (int i = 0; i < array_size; i++) {
        printf("%d ", array_pointer[i]);
    }
    printf("\n");
}

void readArrayFromFile(int *array_pointer, int *array_size_pointer)
{
    FILE *file_pointer = fopen(DATA_FILE, "r");
    if (!file_pointer) {
        perror("Cannot open data file for reading");
        exit(EXIT_FAILURE);
    }

    if (fscanf(file_pointer, "%d", array_size_pointer) != 1) {
        fprintf(stderr, "Error reading array size from file\n");
        fclose(file_pointer);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < *array_size_pointer; i++) {
        if (fscanf(file_pointer, "%d", &array_pointer[i]) != 1) {
            fprintf(stderr, "Error reading element %d from file\n", i);
            fclose(file_pointer);
            exit(EXIT_FAILURE);
        }
    }

    fclose(file_pointer);
}

void writeArrayToFile(int *array_pointer, int array_size)
{
    FILE *file_pointer = fopen(DATA_FILE, "w");
    if (!file_pointer) {
        perror("Cannot open data file for writing");
        exit(EXIT_FAILURE);
    }

    fprintf(file_pointer, "%d\n", array_size);
    for (int i = 0; i < array_size; i++) {
        fprintf(file_pointer, "%d ", array_pointer[i]);
    }
    fprintf(file_pointer, "\n");

    fclose(file_pointer);
}

void sortArrayAscending(int *array_pointer, int array_size)
{
    for (int i = 0; i < array_size - 1; i++) {
        for (int j = 0; j < array_size - i - 1; j++) {
            if (array_pointer[j] > array_pointer[j + 1]) {
                int temp = array_pointer[j];
                array_pointer[j] = array_pointer[j + 1];
                array_pointer[j + 1] = temp;
            }
        }
    }
}

void getUserInputArray(int *array_pointer, int *array_size_pointer)
{
    printf("\nEnter %d integers:\n", *array_size_pointer);
    for (int i = 0; i < *array_size_pointer; i++) {
        char buffer[MAX_BUFFER_SIZE];
        printf("Element %d: ", i + 1);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            fprintf(stderr, "Error reading input\n");
            exit(EXIT_FAILURE);
        }

        if (!validateIntegerInput(buffer)) {
            fprintf(stderr, "Invalid input! Please enter a valid integer.\n");
            i--;
            continue;
        }

        array_pointer[i] = atoi(buffer);
    }
}

void handleValidationAndSort()
{
    int data_array[MAX_ARRAY_SIZE];
    int array_size = 0;

    readArrayFromFile(data_array, &array_size);
    sortArrayAscending(data_array, array_size);
    writeArrayToFile(data_array, array_size);

    printf("\n[%s] Data sorted and written to file\n", PROCESS_2_LABEL);
}


int main()
{
    pid_t child_process_id;
    int user_input_array[MAX_ARRAY_SIZE];
    int actual_array_size = 0;

    printf("\n=== File-Based IPC Mechanism ===\n");

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

    //before sorting
    int display_array[MAX_ARRAY_SIZE];
    readArrayFromFile(display_array, &actual_array_size);
    displayArray(display_array, actual_array_size, "[BEFORE SORTING]");

    // fork child for sorting
    child_process_id = fork();

    if (child_process_id == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (child_process_id == 0) {
        /* Child process: Sort and write to file */
        handleValidationAndSort();
        exit(EXIT_SUCCESS);
    } else {
        /* Parent process: wait for child to finish */
        wait(NULL);

        readArrayFromFile(display_array, &actual_array_size);
        displayArray(display_array, actual_array_size, "[AFTER SORTING]");
    }

    return EXIT_SUCCESS;
}
