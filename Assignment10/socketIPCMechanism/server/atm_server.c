#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define SERVER_PORT 8888
#define LISTEN_BACKLOG 5
#define BUFFER_SIZE 512
#define ACCOUNT_FILE "../resource/accountDB.txt"
#define INITIAL_BALANCE 5000.00
#define MAX_THREADS 10

#define OPERATION_WITHDRAW 1
#define OPERATION_DEPOSIT 2
#define OPERATION_DISPLAY_BALANCE 3
#define OPERATION_EXIT 4

#define RESPONSE_SUCCESS 1
#define RESPONSE_FAILURE 0
#define RESPONSE_INSUFFICIENT_FUNDS 2

pthread_mutex_t file_access_mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for safe file access
volatile int server_running_flag = 1; // Flag to keep server running

typedef struct {
    double current_balance; // Holds current account balance
} AccountData;

typedef struct {
    int operation_code; // Withdraw/Deposit/Display/Exit
    double amount;      // Amount to withdraw/deposit
} TransactionRequest;

typedef struct {
    int response_status;        // SUCCESS/FAILURE/INSUFFICIENT_FUNDS
    double current_balance;     // Current balance after transaction
    char message_text[BUFFER_SIZE]; // Message to send to client
} TransactionResponse;

typedef struct {
    int client_socket_descriptor; // Socket descriptor for client
    int client_identifier;        // Client ID
} ClientThreadArgument;

// Initialize account file with default balance if not exists
void initializeAccountFile()
{
    FILE *file_pointer = fopen(ACCOUNT_FILE, "r");

    if (file_pointer == NULL) {
        file_pointer = fopen(ACCOUNT_FILE, "w");
        if (file_pointer == NULL) {
            perror("Failed to create account file");
            exit(EXIT_FAILURE);
        }

        fprintf(file_pointer, "%.2f\n", INITIAL_BALANCE); // Write initial balance
        fclose(file_pointer);

        printf("Created account file with initial balance: $%.2f\n", INITIAL_BALANCE);
    } else {
        fclose(file_pointer);
        printf("Account file already exists\n");
    }
}

// Read balance from account file
AccountData *readAccountBalance()
{
    AccountData *account_data_pointer = (AccountData *)malloc(sizeof(AccountData));

    if (account_data_pointer == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    FILE *file_pointer = fopen(ACCOUNT_FILE, "r");

    if (file_pointer == NULL) {
        fprintf(stderr, "Cannot open account file for reading\n");
        free(account_data_pointer);
        return NULL;
    }

    if (fscanf(file_pointer, "%lf", &account_data_pointer->current_balance) != 1) {
        fprintf(stderr, "Error reading balance from file\n");
        account_data_pointer->current_balance = 0.0;
    }

    fclose(file_pointer);
    return account_data_pointer;
}

void writeAccountBalance(double new_balance)
{
    FILE *file_pointer = fopen(ACCOUNT_FILE, "w");

    if (file_pointer == NULL) {
        fprintf(stderr, "Cannot open account file for writing\n");
        return;
    }

    fprintf(file_pointer, "%.2f\n", new_balance);
    fclose(file_pointer);
}

void processWithdrawalTransaction(TransactionRequest *request_pointer,
                                 TransactionResponse *response_pointer)
{
    pthread_mutex_lock(&file_access_mutex); // Lock for file access

    AccountData *account_data_pointer = readAccountBalance();

    if (account_data_pointer == NULL) {
        response_pointer->response_status = RESPONSE_FAILURE;
        strcpy(response_pointer->message_text, "System error: Cannot read balance");
        pthread_mutex_unlock(&file_access_mutex);
        return;
    }

    if (request_pointer->amount <= 0) {
        response_pointer->response_status = RESPONSE_FAILURE;
        strcpy(response_pointer->message_text, "Invalid amount: Must be positive");
        response_pointer->current_balance = account_data_pointer->current_balance;
        free(account_data_pointer);
        pthread_mutex_unlock(&file_access_mutex);
        return;
    }

    if (request_pointer->amount > account_data_pointer->current_balance) {
        response_pointer->response_status = RESPONSE_INSUFFICIENT_FUNDS;
        response_pointer->current_balance = account_data_pointer->current_balance;
        snprintf(response_pointer->message_text, BUFFER_SIZE,
                 "Insufficient funds. Available: $%.2f", account_data_pointer->current_balance);
        free(account_data_pointer);
        pthread_mutex_unlock(&file_access_mutex);
        return;
    }

    double new_balance = account_data_pointer->current_balance - request_pointer->amount;
    writeAccountBalance(new_balance);

    response_pointer->response_status = RESPONSE_SUCCESS;
    response_pointer->current_balance = new_balance;
    snprintf(response_pointer->message_text, BUFFER_SIZE,
             "Withdrawal of $%.2f successful", request_pointer->amount);

    free(account_data_pointer);
    pthread_mutex_unlock(&file_access_mutex); // Unlock after file access
}

void processDepositTransaction(TransactionRequest *request_pointer, TransactionResponse *response_pointer)
{
    pthread_mutex_lock(&file_access_mutex);

    AccountData *account_data_pointer = readAccountBalance();

    if (account_data_pointer == NULL) {
        response_pointer->response_status = RESPONSE_FAILURE;
        strcpy(response_pointer->message_text, "System error: Cannot read balance");
        pthread_mutex_unlock(&file_access_mutex);
        return;
    }

    if (request_pointer->amount <= 0) {
        response_pointer->response_status = RESPONSE_FAILURE;
        strcpy(response_pointer->message_text, "Invalid amount: Must be positive");
        response_pointer->current_balance = account_data_pointer->current_balance;
        free(account_data_pointer);
        pthread_mutex_unlock(&file_access_mutex);
        return;
    }

    double new_balance = account_data_pointer->current_balance + request_pointer->amount;
    writeAccountBalance(new_balance);

    response_pointer->response_status = RESPONSE_SUCCESS;
    response_pointer->current_balance = new_balance;
    snprintf(response_pointer->message_text, BUFFER_SIZE,
             "Deposit of $%.2f successful", request_pointer->amount);

    free(account_data_pointer);
    pthread_mutex_unlock(&file_access_mutex);
}

void processDisplayBalanceTransaction(TransactionResponse *response_pointer)
{
    pthread_mutex_lock(&file_access_mutex);

    AccountData *account_data_pointer = readAccountBalance();

    if (account_data_pointer == NULL) {
        response_pointer->response_status = RESPONSE_FAILURE;
        strcpy(response_pointer->message_text, "System error: Cannot read balance");
        pthread_mutex_unlock(&file_access_mutex);
        return;
    }

    response_pointer->response_status = RESPONSE_SUCCESS;
    response_pointer->current_balance = account_data_pointer->current_balance;
    snprintf(response_pointer->message_text, BUFFER_SIZE,
             "Current balance: $%.2f", account_data_pointer->current_balance);

    free(account_data_pointer);
    pthread_mutex_unlock(&file_access_mutex);
}


void *handleClientConnection(void *argument_pointer)
{
    ClientThreadArgument *thread_argument_pointer = (ClientThreadArgument *)argument_pointer;

    int client_socket_descriptor = thread_argument_pointer->client_socket_descriptor;
    int client_identifier = thread_argument_pointer->client_identifier;

    char received_buffer[BUFFER_SIZE];
    TransactionRequest transaction_request;
    TransactionResponse transaction_response;

    printf("[Client %d] Thread started\n", client_identifier);

    while (1) {
        memset(received_buffer, 0, sizeof(received_buffer));

        // Receive request from client 
        ssize_t bytes_received = recv(client_socket_descriptor,
                                      (void *)received_buffer,
                                      sizeof(received_buffer) - 1, 0);

        if (bytes_received <= 0) {
            printf("[Client %d] Disconnected\n", client_identifier);
            break;
        }

        received_buffer[bytes_received] = '\0';

        // Parse request 
        if (sscanf(received_buffer, "%d %lf",
                   &transaction_request.operation_code,
                   &transaction_request.amount) != 2) {
            fprintf(stderr, "[Client %d] Invalid request format\n", client_identifier);
            continue;
        }

        memset(&transaction_response, 0, sizeof(transaction_response));

        // Process transaction 
        switch (transaction_request.operation_code) {
            case OPERATION_WITHDRAW:
                processWithdrawalTransaction(&transaction_request, &transaction_response);
                break;

            case OPERATION_DEPOSIT:
                processDepositTransaction(&transaction_request, &transaction_response);
                break;

            case OPERATION_DISPLAY_BALANCE:
                processDisplayBalanceTransaction(&transaction_response);
                break;

            case OPERATION_EXIT:
                printf("[Client %d] Requested exit\n", client_identifier);
                transaction_response.response_status = RESPONSE_SUCCESS;
                strcpy(transaction_response.message_text, "Goodbye!");
                break;

            default:
                transaction_response.response_status = RESPONSE_FAILURE;
                strcpy(transaction_response.message_text, "Invalid operation code");
                break;
        }

        // Send response to client 
        send(client_socket_descriptor, (const void *)&transaction_response,
             sizeof(transaction_response), 0);

        if (transaction_request.operation_code == OPERATION_EXIT) {
            break;
        }
    }

    close(client_socket_descriptor); // Close client socket
    free(thread_argument_pointer);    // Free memory allocated for thread argument

    return NULL;
}


void handleServerSignal(int signal_number)
{
    printf("\n\nShutting down server...\n");
    server_running_flag = 0;
}

int main()
{
    int server_socket_descriptor;
    int client_socket_descriptor;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    socklen_t client_address_length;
    pthread_t client_thread_id;
    int client_counter = 0;

    printf("\n=== ATM Socket Server ===\n");
    printf("Initializing server...\n\n");

    // Register signal handler
    signal(SIGINT, handleServerSignal);

    // Initialize account file
    initializeAccountFile();

    // Create server socket 
    server_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_descriptor == -1) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    // Set socket options 
    int socket_option_value = 1;
    if (setsockopt(server_socket_descriptor, SOL_SOCKET, SO_REUSEADDR, &socket_option_value, sizeof(socket_option_value)) == -1) {
        perror("Failed to set socket options");
        close(server_socket_descriptor);
        exit(EXIT_FAILURE);
    }

    // Bind socket 
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(SERVER_PORT);

    if (bind(server_socket_descriptor, (struct sockaddr *)&server_address,
             sizeof(server_address)) == -1) {
        perror("Failed to bind socket");
        close(server_socket_descriptor);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket_descriptor, LISTEN_BACKLOG) == -1) {
        perror("Failed to listen on socket");
        close(server_socket_descriptor);
        exit(EXIT_FAILURE);
    }

    printf("Server started on port %d\n", SERVER_PORT);
    printf("Waiting for client connections...\n\n");

    // Main loop to accept clients
    while (server_running_flag) {
        client_address_length = sizeof(client_address);

        client_socket_descriptor = accept(server_socket_descriptor, (struct sockaddr *)&client_address, &client_address_length);

        if (client_socket_descriptor == -1) {
            if (server_running_flag) {
                perror("Failed to accept connection");
            }
            continue;
        }

        client_counter++;
        printf("New client connection [Client ID: %d] from %s:%d\n", 
               client_counter, inet_ntoa(client_address.sin_addr),
               ntohs(client_address.sin_port));

        // Create thread for client
        ClientThreadArgument *thread_argument_pointer = 
            (ClientThreadArgument *)malloc(sizeof(ClientThreadArgument));

        if (thread_argument_pointer == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            close(client_socket_descriptor);
            continue;
        }

        thread_argument_pointer->client_socket_descriptor = client_socket_descriptor;
        thread_argument_pointer->client_identifier = client_counter;

        if (pthread_create(&client_thread_id, NULL, handleClientConnection, (void *)thread_argument_pointer) != 0) {
            perror("Failed to create thread");
            free(thread_argument_pointer);
            close(client_socket_descriptor);
            continue;
        }

        pthread_detach(client_thread_id); // Detach thread to free resources automatically
    }

    close(server_socket_descriptor); // Close server socket
    printf("\nServer shutdown complete\n");

    return EXIT_SUCCESS;
}