/*
 * Socket-Based ATM Client
 * 
 * This program implements a TCP client for ATM transactions.
 * - Connects to ATM server
 * - Provides menu-driven interface for transactions
 * - Handles Withdraw, Deposit, and Display Balance operations
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* ============= MACRO DEFINITIONS ============= */
#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 8888
#define BUFFER_SIZE 512
#define MENU_OPTION_WITHDRAW 1
#define MENU_OPTION_DEPOSIT 2
#define MENU_OPTION_DISPLAY_BALANCE 3
#define MENU_OPTION_EXIT 4

/* ============= OPERATION CODES ============= */
#define OPERATION_WITHDRAW 1
#define OPERATION_DEPOSIT 2
#define OPERATION_DISPLAY_BALANCE 3
#define OPERATION_EXIT 4

/* ============= RESPONSE CODES ============= */
#define RESPONSE_SUCCESS 1
#define RESPONSE_FAILURE 0
#define RESPONSE_INSUFFICIENT_FUNDS 2

/* ============= STRUCTURE DEFINITIONS ============= */
typedef struct {
    int operation_code;
    double amount;
} TransactionRequest;

typedef struct {
    int response_status;
    double current_balance;
    char message_text[BUFFER_SIZE];
} TransactionResponse;

/* ============= FUNCTION DECLARATIONS ============= */
int connectToServer();
void displayMainMenu();
void handleWithdrawalOperation(int server_socket_descriptor);
void handleDepositOperation(int server_socket_descriptor);
void handleDisplayBalanceOperation(int server_socket_descriptor);
int validateAmountInput(const char *user_input_string);
void displayTransactionResult(const TransactionResponse *response_pointer);
void clearInputBuffer();

/* ============= MAIN FUNCTION ============= */
int main()
{
    int server_socket_descriptor;
    int user_menu_choice;
    char user_input_buffer[BUFFER_SIZE];

    printf("\n=== ATM Client ===\n");
    printf("Attempting to connect to server at %s:%d\n\n", SERVER_ADDRESS, SERVER_PORT);

    /* Connect to server */
    server_socket_descriptor = connectToServer();

    if (server_socket_descriptor == -1) {
        fprintf(stderr, "Failed to connect to server\n");
        exit(EXIT_FAILURE);
    }

    printf("Successfully connected to server\n\n");

    /* Main transaction loop */
    while (1) {
        displayMainMenu();

        printf("Enter your choice (1-4): ");
        if (fgets(user_input_buffer, sizeof(user_input_buffer), stdin) == NULL) {
            fprintf(stderr, "Error reading input\n");
            continue;
        }

        /* Validate menu choice input */
        if (sscanf(user_input_buffer, "%d", &user_menu_choice) != 1) {
            fprintf(stderr, "Invalid input. Please enter a number between 1 and 4.\n\n");
            continue;
        }

        if (user_menu_choice < MENU_OPTION_WITHDRAW || user_menu_choice > MENU_OPTION_EXIT) {
            fprintf(stderr, "Invalid choice. Please enter a number between 1 and 4.\n\n");
            continue;
        }

        /* Process menu choice */
        switch (user_menu_choice) {
            case MENU_OPTION_WITHDRAW:
                handleWithdrawalOperation(server_socket_descriptor);
                break;

            case MENU_OPTION_DEPOSIT:
                handleDepositOperation(server_socket_descriptor);
                break;

            case MENU_OPTION_DISPLAY_BALANCE:
                handleDisplayBalanceOperation(server_socket_descriptor);
                break;

            case MENU_OPTION_EXIT:
                printf("\nProcessing exit request...\n");
                handleDisplayBalanceOperation(server_socket_descriptor);

                /* Send exit signal */
                TransactionRequest exit_request = {OPERATION_EXIT, 0.0};
                send(server_socket_descriptor, (const void *)&exit_request,
                     sizeof(exit_request), 0);

                TransactionResponse exit_response;
                recv(server_socket_descriptor, (void *)&exit_response,
                     sizeof(exit_response), 0);

                printf("Thank you for using ATM. Goodbye!\n\n");
                close(server_socket_descriptor);
                return EXIT_SUCCESS;

            default:
                fprintf(stderr, "Unexpected error\n");
                break;
        }
    }

    close(server_socket_descriptor);
    return EXIT_SUCCESS;
}

/* ============= FUNCTION IMPLEMENTATIONS ============= */

/**
 * Establishes TCP connection to server
 */
int connectToServer()
{
    int client_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (client_socket_descriptor == -1) {
        perror("Failed to create socket");
        return -1;
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_ADDRESS, &server_address.sin_addr) <= 0) {
        perror("Invalid server address");
        close(client_socket_descriptor);
        return -1;
    }

    if (connect(client_socket_descriptor, 
                (struct sockaddr *)&server_address,
                sizeof(server_address)) == -1) {
        perror("Failed to connect to server");
        close(client_socket_descriptor);
        return -1;
    }

    return client_socket_descriptor;
}

/**
 * Displays main menu options
 */
void displayMainMenu()
{
    printf("\n--- ATM Menu ---\n");
    printf("1. Withdraw Amount\n");
    printf("2. Deposit Amount\n");
    printf("3. Display Current Balance\n");
    printf("4. Exit\n");
}

/**
 * Handles withdrawal operation
 */
void handleWithdrawalOperation(int server_socket_descriptor)
{
    char amount_input_buffer[BUFFER_SIZE];
    double withdrawal_amount;
    TransactionResponse transaction_response;
    char send_buffer[BUFFER_SIZE];

    printf("\n--- Withdrawal ---\n");
    printf("Enter amount to withdraw: $");

    if (fgets(amount_input_buffer, sizeof(amount_input_buffer), stdin) == NULL) {
        fprintf(stderr, "Error reading input\n");
        return;
    }

    if (!validateAmountInput(amount_input_buffer)) {
        fprintf(stderr, "Invalid amount. Please enter a positive number.\n\n");
        return;
    }

    withdrawal_amount = atof(amount_input_buffer);

    if (withdrawal_amount <= 0) {
        fprintf(stderr, "Amount must be greater than zero.\n\n");
        return;
    }

    snprintf(send_buffer, sizeof(send_buffer), "%d %.2f",
             OPERATION_WITHDRAW, withdrawal_amount);

    if (send(server_socket_descriptor, send_buffer,
             strlen(send_buffer), 0) == -1) {
        perror("Failed to send request to server");
        return;
    }

    /* Receive response (STRUCT from server) */
    if (recv(server_socket_descriptor, (void *)&transaction_response,
             sizeof(transaction_response), 0) == -1) {
        perror("Failed to receive response from server");
        return;
    }

    displayTransactionResult(&transaction_response);
}

/**
 * Handles deposit operation
 */
void handleDepositOperation(int server_socket_descriptor)
{
    char amount_input_buffer[BUFFER_SIZE];
    double deposit_amount;
    TransactionRequest transaction_request;
    TransactionResponse transaction_response;

    printf("\n--- Deposit ---\n");
    printf("Enter amount to deposit: $");

    if (fgets(amount_input_buffer, sizeof(amount_input_buffer), stdin) == NULL) {
        fprintf(stderr, "Error reading input\n");
        return;
    }

    if (!validateAmountInput(amount_input_buffer)) {
        fprintf(stderr, "Invalid amount. Please enter a positive number.\n\n");
        return;
    }

    deposit_amount = atof(amount_input_buffer);

    if (deposit_amount <= 0) {
        fprintf(stderr, "Amount must be greater than zero.\n\n");
        return;
    }

    /* Create and send request */
    transaction_request.operation_code = OPERATION_DEPOSIT;
    transaction_request.amount = deposit_amount;

    if (send(server_socket_descriptor, (const void *)&transaction_request,
             sizeof(transaction_request), 0) == -1) {
        perror("Failed to send request to server");
        return;
    }

    /* Receive response */
    if (recv(server_socket_descriptor, (void *)&transaction_response,
             sizeof(transaction_response), 0) == -1) {
        perror("Failed to receive response from server");
        return;
    }

    displayTransactionResult(&transaction_response);
}

/**
 * Handles display balance operation
 */
void handleDisplayBalanceOperation(int server_socket_descriptor)
{
    TransactionResponse transaction_response;
    char send_buffer[BUFFER_SIZE];

    printf("\n--- Display Balance ---\n");

    /* ✅ Send as TEXT */
    snprintf(send_buffer, sizeof(send_buffer), "%d 0",
             OPERATION_DISPLAY_BALANCE);

    if (send(server_socket_descriptor, send_buffer,
             strlen(send_buffer), 0) == -1) {
        perror("Failed to send request to server");
        return;
    }

    /* Receive response STRUCT */
    if (recv(server_socket_descriptor,
             (void *)&transaction_response,
             sizeof(transaction_response), 0) == -1) {
        perror("Failed to receive response from server");
        return;
    }

    displayTransactionResult(&transaction_response);
}


/**
 * Validates if string contains valid amount
 */
int validateAmountInput(const char *user_input_string)
{
    if (user_input_string == NULL || *user_input_string == '\0') {
        return 0;
    }

    int decimal_point_count = 0;
    int digit_count = 0;

    for (int index = 0; user_input_string[index] != '\0'; index++) {
        char current_character = user_input_string[index];

        if (current_character == '\n') {
            break;
        }

        if (current_character >= '0' && current_character <= '9') {
            digit_count++;
        } else if (current_character == '.' || current_character == ',') {
            decimal_point_count++;
            if (decimal_point_count > 1) {
                return 0;
            }
        } else if (current_character == '-' && index == 0) {
            return 0;
        } else if (current_character == ' ') {
            continue;
        } else {
            return 0;
        }
    }

    return (digit_count > 0) ? 1 : 0;
}

/**
 * Displays transaction result
 */
void displayTransactionResult(const TransactionResponse *response_pointer)
{
    printf("\n--- Transaction Result ---\n");

    if (response_pointer->response_status == RESPONSE_SUCCESS) {
        printf("Status: SUCCESS ✓\n");
    } else if (response_pointer->response_status == RESPONSE_INSUFFICIENT_FUNDS) {
        printf("Status: INSUFFICIENT FUNDS ✗\n");
    } else {
        printf("Status: FAILURE ✗\n");
    }

    printf("Message: %s\n", response_pointer->message_text);
    printf("Current Balance: $%.2f\n\n", response_pointer->current_balance);
}

/**
 * Clears input buffer
 */
void clearInputBuffer()
{
    int character;
    while ((character = getchar()) != '\n' && character != EOF);
}