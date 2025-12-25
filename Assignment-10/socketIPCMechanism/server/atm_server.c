#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 8080
#define ACCOUNT_FILE "../resource/accountDB.txt"

pthread_mutex_t fileMutex;

int readBalance() {
    int balance = 0;
    FILE *file = fopen(ACCOUNT_FILE, "r");
    if (file != NULL) {
        fscanf(file, "%d", &balance);
        fclose(file);
    } else { 
        file = fopen(ACCOUNT_FILE, "w");
        if (file) {
            fprintf(file, "0");
            fclose(file);
        } else {
            perror("Failed to create account file");
        }
    }
    return balance;
}

void writeBalance(int balance) {
    FILE *file = fopen(ACCOUNT_FILE, "w");
    if (file != NULL) {
        fprintf(file, "%d", balance);
        fclose(file);
    } else {
        perror("Failed to write to account file");
    }
}

void *handleClient(void *clientSocketPtr) {
    int clientSocket = *(int *)clientSocketPtr;
    free(clientSocketPtr); 

    int choice, amount;
    char response[100];
    int balance;

    if (recv(clientSocket, &choice, sizeof(choice), 0) <= 0) {
        close(clientSocket);
        return NULL;
    }

    pthread_mutex_lock(&fileMutex); // lock for safe file access
    balance = readBalance();

    switch (choice) {
        case 1: // Withdraw
            if (recv(clientSocket, &amount, sizeof(amount), 0) <= 0) break;
            if (amount > balance) {
                sprintf(response, "Withdrawal failed: Insufficient balance.");
            } else {
                balance -= amount;
                writeBalance(balance);
                sprintf(response, "Withdrawal successful. New Balance: %d", balance);
            }
            break;

        case 2: // Deposit
            if (recv(clientSocket, &amount, sizeof(amount), 0) <= 0) break;
            balance += amount;
            writeBalance(balance);
            sprintf(response, "Deposit successful. New Balance: %d", balance);
            break;

        case 3: // Display Balance
            sprintf(response, "Current Balance: %d", balance);
            break;

        default:
            strcpy(response, "Invalid choice.");
    }

    pthread_mutex_unlock(&fileMutex); // unlock

    if (send(clientSocket, response, strlen(response) + 1, 0) < 0) { //send response to
        perror("Failed to send response to client");
    }

    close(clientSocket); // close client socket
    return NULL;
}

int main() {
    int serverSocket;
    struct sockaddr_in serverAddress;
    pthread_t threadId;

    pthread_mutex_init(&fileMutex, NULL); // initialize mutex

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(serverSocket);
        exit(1);
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Bind failed");
        close(serverSocket);
        exit(1);
    }

    if (listen(serverSocket, 5) < 0) {
        perror("Listen failed");
        close(serverSocket);
        exit(1);
    }

    printf("ATM Server running on port %d...\n", PORT);

    while (1) {
        int clientSocket = accept(serverSocket, NULL, NULL); //accept client request
        if (clientSocket < 0) {
            perror("Accept failed");
            continue;
        }
        int *pClient = malloc(sizeof(int));
        if (!pClient) {
            perror("Memory allocation failed");
            close(clientSocket);
            continue;
        }
        *pClient = clientSocket;

        if (pthread_create(&threadId, NULL, handleClient, pClient) != 0) {
            perror("Thread creation failed");
            free(pClient);
            close(clientSocket);
        } else {
            pthread_detach(threadId);
        }
    }

    close(serverSocket);
    pthread_mutex_destroy(&fileMutex);
    return 0;
}
