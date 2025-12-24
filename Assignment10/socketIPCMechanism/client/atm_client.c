#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 8888
#define BUFFER_SIZE 512
#define MENU_OPTION_WITHDRAW 1
#define MENU_OPTION_DEPOSIT 2
#define MENU_OPTION_DISPLAY_BALANCE 3
#define MENU_OPTION_EXIT 4

#define OPERATION_WITHDRAW 1
#define OPERATION_DEPOSIT 2
#define OPERATION_DISPLAY_BALANCE 3
#define OPERATION_EXIT 4

#define RESPONSE_SUCCESS 1
#define RESPONSE_FAILURE 0
#define RESPONSE_INSUFFICIENT_FUNDS 2

typedef struct {
    int operation_code;
    double amount;
} TransactionRequest;

typedef struct {
    int response_status;
    double current_balance;
    char message_text[BUFFER_SIZE];
} TransactionResponse;

int connectToServer() {
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

void displayMainMenu() {
    printf("\n--- ATM Menu ---\n");
    printf("1. Withdraw Amount\n");
    printf("2. Deposit Amount\n");
    printf("3. Display Current Balance\n");
    printf("4. Exit\n");
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int validateAmountInput(const char *user_input_string) {
    if (!user_input_string || *user_input_string == '\0') return 0;

    int decimal_count = 0;
    int digit_count = 0;
    for (int i = 0; user_input_string[i] != '\0'; i++) {
        char c = user_input_string[i];
        if (c == '\n') break;
        if (c >= '0' && c <= '9') digit_count++;
        else if (c == '.') decimal_count++;
        else if (c == ' ' || c == ',') continue;
        else return 0;
        if (decimal_count > 1) return 0;
    }
    return (digit_count > 0);
}

void displayTransactionResult(const TransactionResponse *response_pointer) {
    printf("\n--- Transaction Result ---\n");

    if (response_pointer->response_status == RESPONSE_SUCCESS) {
        printf("Status: SUCCESS\n");
    } else if (response_pointer->response_status == RESPONSE_INSUFFICIENT_FUNDS) {
        printf("Status: INSUFFICIENT FUNDS ✗\n");
    } else {
        printf("Status: FAILURE\n");
    }

    printf("Message: %s\n", response_pointer->message_text);
    printf("Current Balance: $%.2f\n\n", response_pointer->current_balance);
}

void handleWithdrawalOperation(int server_socket_descriptor) {
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
        fprintf(stderr, "Invalid amount. Please enter a positive number.\n");
        return;
    }

    withdrawal_amount = atof(amount_input_buffer);
    if (withdrawal_amount <= 0) {
        fprintf(stderr, "Amount must be greater than zero.\n");
        return;
    }

    snprintf(send_buffer, sizeof(send_buffer), "%d %.2f",
             OPERATION_WITHDRAW, withdrawal_amount);

    if (send(server_socket_descriptor, send_buffer, strlen(send_buffer), 0) == -1) {
        perror("Failed to send request to server");
        return;
    }

    if (recv(server_socket_descriptor, (void *)&transaction_response, sizeof(transaction_response), 0) <= 0) {
        perror("Failed to receive response from server");
        return;
    }

    displayTransactionResult(&transaction_response);
}

void handleDepositOperation(int server_socket_descriptor) {
    char amount_input_buffer[BUFFER_SIZE];
    double deposit_amount;
    TransactionResponse transaction_response;
    char send_buffer[BUFFER_SIZE];

    printf("\n--- Deposit ---\n");
    printf("Enter amount to deposit: $");

    if (fgets(amount_input_buffer, sizeof(amount_input_buffer), stdin) == NULL) {
        fprintf(stderr, "Error reading input\n");
        return;
    }

    if (!validateAmountInput(amount_input_buffer)) {
        fprintf(stderr, "Invalid amount. Please enter a positive number.\n");
        return;
    }

    deposit_amount = atof(amount_input_buffer);
    if (deposit_amount <= 0) {
        fprintf(stderr, "Amount must be greater than zero.\n");
        return;
    }

    snprintf(send_buffer, sizeof(send_buffer), "%d %.2f",
             OPERATION_DEPOSIT, deposit_amount);

    if (send(server_socket_descriptor, send_buffer, strlen(send_buffer), 0) == -1) {
        perror("Failed to send request to server");
        return;
    }

    if (recv(server_socket_descriptor, (void *)&transaction_response, sizeof(transaction_response), 0) <= 0) {
        perror("Failed to receive response from server");
        return;
    }

    displayTransactionResult(&transaction_response);
}

void handleDisplayBalanceOperation(int server_socket_descriptor) {
    TransactionResponse transaction_response;
    char send_buffer[BUFFER_SIZE];

    snprintf(send_buffer, sizeof(send_buffer), "%d 0", OPERATION_DISPLAY_BALANCE);

    if (send(server_socket_descriptor, send_buffer, strlen(send_buffer), 0) == -1) {
        perror("Failed to send request to server");
        return;
    }

    if (recv(server_socket_descriptor, (void *)&transaction_response, sizeof(transaction_response), 0) <= 0) {
        perror("Failed to receive response from server");
        return;
    }

    displayTransactionResult(&transaction_response);
}

int main() {
    int server_socket_descriptor;
    int user_menu_choice;
    char user_input_buffer[BUFFER_SIZE];

    printf("\n=== ATM Client ===\n");
    printf("Attempting to connect to server at %s:%d\n\n", SERVER_ADDRESS, SERVER_PORT);

    server_socket_descriptor = connectToServer();
    if (server_socket_descriptor == -1) {
        fprintf(stderr, "Failed to connect to server\n");
        exit(EXIT_FAILURE);
    }

    printf("Successfully connected to server\n");

    while (1) {
        displayMainMenu();
        printf("Enter your choice (1-4): ");

        if (fgets(user_input_buffer, sizeof(user_input_buffer), stdin) == NULL) {
            fprintf(stderr, "Error reading input\n");
            continue;
        }

        if (sscanf(user_input_buffer, "%d", &user_menu_choice) != 1) {
            fprintf(stderr, "Invalid input. Please enter a number between 1 and 4.\n");
            continue;
        }

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
            case MENU_OPTION_EXIT: {
                char send_buffer[BUFFER_SIZE];
                snprintf(send_buffer, sizeof(send_buffer), "%d 0", OPERATION_EXIT);
                send(server_socket_descriptor, send_buffer, strlen(send_buffer), 0);

                TransactionResponse exit_response;
                recv(server_socket_descriptor, &exit_response, sizeof(exit_response), 0);
                printf("%s\n", exit_response.message_text);

                close(server_socket_descriptor);
                printf("Thank you for using ATM. Goodbye!\n");
                return EXIT_SUCCESS;
            }
            default:
                fprintf(stderr, "Invalid choice.\n");
        }
    }

    close(server_socket_descriptor);
    return EXIT_SUCCESS;
}
