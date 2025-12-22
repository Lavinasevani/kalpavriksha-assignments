/*
 * Message Queue-Based IPC Mechanism
 * 
 * This program demonstrates interprocess communication using System V message queues.
 * Parent Process: Reads array, sends to child via message queue
 * Child Process: Sorts array, sends back via message queue
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <errno.h>

/* ============= MACRO DEFINITIONS ============= */
#define MAX_ARRAY_SIZE 50
#define MAX_BUFFER_SIZE 1024
#define MESSAGE_KEY_SEND 5555
#define MESSAGE_KEY_RECEIVE 6666
#define MESSAGE_TYPE_ARRAY 1
#define MESSAGE_TYPE_RESPONSE 2
#define PARENT_PROCESS_LABEL "PARENT"
#define CHILD_PROCESS_LABEL "CHILD"
#define SUCCESS_EXIT_CODE 0
#define FAILURE_EXIT_CODE 1

/* ============= STRUCTURE DEFINITIONS ============= */
typedef struct {
    long message_type;
    int array_size;
    int array_elements[MAX_ARRAY_SIZE];
} MessageQueuePacket;

/* ============= FUNCTION DECLARATIONS ============= */
void displayArray(int *array_pointer, int array_size, const char *stage_label);
void sortArrayAscending(int *array_pointer, int array_size);
void getUserInputArray(int *array_pointer, int *array_size_pointer);
void handleParentProcess();
void handleChildProcess();
int validateIntegerInput(const char *user_input_string);
void sendArrayViaMessageQueue(int message_queue_id, MessageQueuePacket *packet_pointer);
void receiveArrayViaMessageQueue(int message_queue_id, MessageQueuePacket *packet_pointer, 
                                 long message_type);

/* ============= MAIN FUNCTION ============= */
int main()
{
    pid_t child_process_id;
    int message_queue_send_id;
    int message_queue_receive_id;

    printf("\n=== Message Queue-Based IPC Mechanism ===\n\n");

    /* Create message queues */
    message_queue_send_id = msgget(MESSAGE_KEY_SEND, IPC_CREAT | 0666);
    if (message_queue_send_id == -1) {
        perror("Failed to create send message queue");
        exit(FAILURE_EXIT_CODE);
    }

    message_queue_receive_id = msgget(MESSAGE_KEY_RECEIVE, IPC_CREAT | 0666);
    if (message_queue_receive_id == -1) {
        perror("Failed to create receive message queue");
        msgctl(message_queue_send_id, IPC_RMID, NULL);
        exit(FAILURE_EXIT_CODE);
    }

    /* Fork child process */
    child_process_id = fork();

    if (child_process_id == -1) {
        perror("fork failed");
        msgctl(message_queue_send_id, IPC_RMID, NULL);
        msgctl(message_queue_receive_id, IPC_RMID, NULL);
        exit(FAILURE_EXIT_CODE);
    }

    if (child_process_id == 0) {
        /* Child process */
        handleChildProcess();
        exit(SUCCESS_EXIT_CODE);
    } else {
        /* Parent process */
        handleParentProcess();
        wait(NULL);

        /* Clean up message queues */
        msgctl(message_queue_send_id, IPC_RMID, NULL);
        msgctl(message_queue_receive_id, IPC_RMID, NULL);
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
 * Sends array via message queue
 */
void sendArrayViaMessageQueue(int message_queue_id, MessageQueuePacket *packet_pointer)
{
    if (msgsnd(message_queue_id, (void *)packet_pointer, 
               sizeof(MessageQueuePacket) - sizeof(long), 0) == -1) {
        perror("Failed to send message");
        exit(FAILURE_EXIT_CODE);
    }
}

/**
 * Receives array via message queue
 */
void receiveArrayViaMessageQueue(int message_queue_id, MessageQueuePacket *packet_pointer,
                                 long message_type)
{
    if (msgrcv(message_queue_id, (void *)packet_pointer, 
               sizeof(MessageQueuePacket) - sizeof(long), message_type, 0) == -1) {
        perror("Failed to receive message");
        exit(FAILURE_EXIT_CODE);
    }
}

/**
 * Parent process handler
 */
void handleParentProcess()
{
    int message_queue_send_id;
    int message_queue_receive_id;
    MessageQueuePacket send_packet = {0};
    MessageQueuePacket receive_packet = {0};
    int user_input_array[MAX_ARRAY_SIZE];
    char size_input_buffer[MAX_BUFFER_SIZE];

    message_queue_send_id = msgget(MESSAGE_KEY_SEND, 0);
    message_queue_receive_id = msgget(MESSAGE_KEY_RECEIVE, 0);

    if (message_queue_send_id == -1 || message_queue_receive_id == -1) {
        fprintf(stderr, "Failed to access message queues\n");
        exit(FAILURE_EXIT_CODE);
    }

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

    send_packet.array_size = atoi(size_input_buffer);

    if (send_packet.array_size <= 0 || send_packet.array_size > MAX_ARRAY_SIZE) {
        fprintf(stderr, "Array size must be between 1 and %d\n", MAX_ARRAY_SIZE);
        exit(FAILURE_EXIT_CODE);
    }

    getUserInputArray(user_input_array, &send_packet.array_size);

    /* Copy array to packet */
    for (int index = 0; index < send_packet.array_size; index++) {
        send_packet.array_elements[index] = user_input_array[index];
    }

    displayArray(user_input_array, send_packet.array_size, "[BEFORE SORTING]");

    /* Send array to child process */
    send_packet.message_type = MESSAGE_TYPE_ARRAY;
    printf("\n[%s] Sending array via message queue...\n", PARENT_PROCESS_LABEL);
    sendArrayViaMessageQueue(message_queue_send_id, &send_packet);

    /* Receive sorted array from child */
    printf("[%s] Waiting for sorted array...\n", PARENT_PROCESS_LABEL);
    receiveArrayViaMessageQueue(message_queue_receive_id, &receive_packet, MESSAGE_TYPE_RESPONSE);

    /* Extract sorted array */
    for (int index = 0; index < receive_packet.array_size; index++) {
        user_input_array[index] = receive_packet.array_elements[index];
    }

    displayArray(user_input_array, receive_packet.array_size, "[AFTER SORTING]");

    printf("\n[%s] Completed\n", PARENT_PROCESS_LABEL);
}

/**
 * Child process handler
 */
void handleChildProcess()
{
    int message_queue_send_id;
    int message_queue_receive_id;
    MessageQueuePacket receive_packet = {0};
    MessageQueuePacket send_packet = {0};

    message_queue_send_id = msgget(MESSAGE_KEY_SEND, 0);
    message_queue_receive_id = msgget(MESSAGE_KEY_RECEIVE, 0);

    if (message_queue_send_id == -1 || message_queue_receive_id == -1) {
        fprintf(stderr, "[%s] Failed to access message queues\n", CHILD_PROCESS_LABEL);
        exit(FAILURE_EXIT_CODE);
    }

    printf("[%s] Started and waiting for data...\n", CHILD_PROCESS_LABEL);

    /* Receive array from parent */
    receiveArrayViaMessageQueue(message_queue_send_id, &receive_packet, MESSAGE_TYPE_ARRAY);

    printf("[%s] Received array of size %d\n", CHILD_PROCESS_LABEL, receive_packet.array_size);

    /* Sort the array */
    sortArrayAscending(receive_packet.array_elements, receive_packet.array_size);

    printf("[%s] Array sorted, sending back to parent\n", CHILD_PROCESS_LABEL);

    /* Prepare and send response */
    send_packet.message_type = MESSAGE_TYPE_RESPONSE;
    send_packet.array_size = receive_packet.array_size;

    for (int index = 0; index < receive_packet.array_size; index++) {
        send_packet.array_elements[index] = receive_packet.array_elements[index];
    }

    sendArrayViaMessageQueue(message_queue_receive_id, &send_packet);

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