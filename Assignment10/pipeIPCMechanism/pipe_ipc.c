/*
 * Pipe-Based IPC Mechanism
 * 
 * This program demonstrates interprocess communication using pipes.
 * Parent Process: Reads array, displays before and after sorting
 * Child Process: Receives array through pipe, sorts it, sends back through pipe
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/* ============= MACRO DEFINITIONS ============= */
#define MAX_ARRAY_SIZE 100
#define MAX_BUFFER_SIZE 1024
#define PIPE_READ_END 0
#define PIPE_WRITE_END 1
#define PARENT_PROCESS_LABEL "PARENT_PROCESS"
#define CHILD_PROCESS_LABEL "CHILD_PROCESS"
#define SUCCESS_EXIT_CODE 0
#define FAILURE_EXIT_CODE 1

/* ============= STRUCTURE DEFINITION ============= */
typedef struct {
    int array_size;
    int array_elements[MAX_ARRAY_SIZE];
} ArrayPacket;

/* ============= FUNCTION DECLARATIONS ============= */
void displayArray(int *array_pointer, int array_size, const char *stage_label);
void sortArrayAscending(int *array_pointer, int array_size);
void getUserInputArray(int *array_pointer, int *array_size_pointer);
void handleParentProcess(int parent_to_child_pipe[2], int child_to_parent_pipe[2]);
void handleChildProcess(int parent_to_child_pipe[2], int child_to_parent_pipe[2]);
int validateIntegerInput(const char *user_input_string);
void writePacketToPipe(int pipe_file_descriptor, ArrayPacket *packet_pointer);
void readPacketFromPipe(int pipe_file_descriptor, ArrayPacket *packet_pointer);

/* ============= MAIN FUNCTION ============= */
int main()
{
    pid_t child_process_id;
    int parent_to_child_pipe_descriptor[2];
    int child_to_parent_pipe_descriptor[2];

    printf("\n=== Pipe-Based IPC Mechanism ===\n\n");

    /* Create pipes */
    if (pipe(parent_to_child_pipe_descriptor) == -1) {
        perror("Failed to create parent-to-child pipe");
        exit(FAILURE_EXIT_CODE);
    }

    if (pipe(child_to_parent_pipe_descriptor) == -1) {
        perror("Failed to create child-to-parent pipe");
        exit(FAILURE_EXIT_CODE);
    }

    /* Fork child process */
    child_process_id = fork();

    if (child_process_id == -1) {
        perror("fork failed");
        exit(FAILURE_EXIT_CODE);
    }

    if (child_process_id == 0) {
        /* Child process */
        handleChildProcess(parent_to_child_pipe_descriptor, 
                          child_to_parent_pipe_descriptor);
        exit(SUCCESS_EXIT_CODE);
    } else {
        /* Parent process */
        handleParentProcess(parent_to_child_pipe_descriptor, 
                           child_to_parent_pipe_descriptor);
        wait(NULL);
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
 * Writes packet structure to pipe
 */
void writePacketToPipe(int pipe_file_descriptor, ArrayPacket *packet_pointer)
{
    ssize_t bytes_written = write(pipe_file_descriptor, (const void *)packet_pointer, 
                                  sizeof(ArrayPacket));
    
    if (bytes_written == -1) {
        perror("Failed to write to pipe");
        exit(FAILURE_EXIT_CODE);
    }

    if (bytes_written != sizeof(ArrayPacket)) {
        fprintf(stderr, "Warning: Incomplete write to pipe\n");
    }
}

/**
 * Reads packet structure from pipe
 */
void readPacketFromPipe(int pipe_file_descriptor, ArrayPacket *packet_pointer)
{
    ssize_t bytes_read = read(pipe_file_descriptor, (void *)packet_pointer, 
                              sizeof(ArrayPacket));
    
    if (bytes_read == -1) {
        perror("Failed to read from pipe");
        exit(FAILURE_EXIT_CODE);
    }

    if (bytes_read == 0) {
        fprintf(stderr, "Pipe closed unexpectedly\n");
        exit(FAILURE_EXIT_CODE);
    }

    if (bytes_read != sizeof(ArrayPacket)) {
        fprintf(stderr, "Warning: Incomplete read from pipe\n");
    }
}

/**
 * Parent process handler
 */
void handleParentProcess(int parent_to_child_pipe[2], int child_to_parent_pipe[2])
{
    ArrayPacket data_packet = {0};
    int user_input_array[MAX_ARRAY_SIZE];
    char size_input_buffer[MAX_BUFFER_SIZE];

    printf("[%s] Started\n", PARENT_PROCESS_LABEL);

    /* Close unused pipe ends */
    close(parent_to_child_pipe[PIPE_READ_END]);
    close(child_to_parent_pipe[PIPE_WRITE_END]);

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

    data_packet.array_size = atoi(size_input_buffer);

    if (data_packet.array_size <= 0 || data_packet.array_size > MAX_ARRAY_SIZE) {
        fprintf(stderr, "Array size must be between 1 and %d\n", MAX_ARRAY_SIZE);
        exit(FAILURE_EXIT_CODE);
    }

    getUserInputArray(user_input_array, &data_packet.array_size);

    /* Copy array to packet */
    for (int index = 0; index < data_packet.array_size; index++) {
        data_packet.array_elements[index] = user_input_array[index];
    }

    displayArray(user_input_array, data_packet.array_size, "[BEFORE SORTING]");

    /* Send array to child process */
    printf("\n[%s] Sending data to child process...\n", PARENT_PROCESS_LABEL);
    writePacketToPipe(parent_to_child_pipe[PIPE_WRITE_END], &data_packet);

    /* Wait for sorted array from child */
    printf("[%s] Waiting for sorted data from child...\n", PARENT_PROCESS_LABEL);
    readPacketFromPipe(child_to_parent_pipe[PIPE_READ_END], &data_packet);

    /* Extract sorted array */
    for (int index = 0; index < data_packet.array_size; index++) {
        user_input_array[index] = data_packet.array_elements[index];
    }

    displayArray(user_input_array, data_packet.array_size, "[AFTER SORTING]");

    /* Close pipe ends */
    close(parent_to_child_pipe[PIPE_WRITE_END]);
    close(child_to_parent_pipe[PIPE_READ_END]);

    printf("\n[%s] Completed\n", PARENT_PROCESS_LABEL);
}

/**
 * Child process handler
 */
void handleChildProcess(int parent_to_child_pipe[2], int child_to_parent_pipe[2])
{
    ArrayPacket data_packet = {0};

    /* Close unused pipe ends */
    close(parent_to_child_pipe[PIPE_WRITE_END]);
    close(child_to_parent_pipe[PIPE_READ_END]);

    printf("[%s] Started and waiting for data...\n", CHILD_PROCESS_LABEL);

    /* Read array from parent */
    readPacketFromPipe(parent_to_child_pipe[PIPE_READ_END], &data_packet);

    printf("[%s] Received array of size %d\n", CHILD_PROCESS_LABEL, data_packet.array_size);

    /* Sort the array */
    sortArrayAscending(data_packet.array_elements, data_packet.array_size);

    printf("[%s] Array sorted, sending back to parent\n", CHILD_PROCESS_LABEL);

    /* Send sorted array back to parent */
    writePacketToPipe(child_to_parent_pipe[PIPE_WRITE_END], &data_packet);

    /* Close pipe ends */
    close(parent_to_child_pipe[PIPE_READ_END]);
    close(child_to_parent_pipe[PIPE_WRITE_END]);

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