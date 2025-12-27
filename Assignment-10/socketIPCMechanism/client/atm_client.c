#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

void displayMenu() {
    printf("\n---- ATM Menu ----\n1. Withdraw\n2. Deposit\n3. Display Balance\n4. Exit\n");
    return ;
}

int main() {
    int clientSocket;
    struct sockaddr_in serverAddress;
    int choice, amount;
    char response[100];

    clientSocket = socket(AF_INET, SOCK_STREAM, 0); // create TCP socket for checking server avaliablity
    if (clientSocket < 0) {
        printf("Socket creation failed");
        exit(1);
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT); // host to network short
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        printf("Connection failed"); 
        close(clientSocket);
        exit(1); 
    }
    close(clientSocket); // close that socket

    while (1) {

        displayMenu();
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            exit(1);
        }

        if (choice == 4) {
            printf("Thank you for using ATM Service.\n");
            return 0;
        }

        if (choice < 1 || choice > 3) {
            printf("Invalid choice. Try again.\n");
            continue;
        }
        //socket(int domain[AF_INET means IPv4], int type[SOCK_STREAM menas TCP], int protocol[0 means system chooses TCP])
        clientSocket = socket(AF_INET, SOCK_STREAM, 0); // create TCP socket
        
        if (clientSocket < 0) {
            printf("Socket creation failed\n");
            exit(1);
        }

        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(PORT); //host to network short
        serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); //convert "127.0.0.1" into a 32-bit binary

        if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
            printf("Connection failed\n");
            close(clientSocket);
            continue;
        }

        send(clientSocket, &choice, sizeof(choice), 0); //client to server request with choice

        if (choice == 1 || choice == 2) {
            printf("Enter amount: ");
            scanf("%d", &amount);
            send(clientSocket, &amount, sizeof(amount), 0);
        }

        recv(clientSocket, response, sizeof(response), 0); //recive response from server
        printf("\nServer Response: %s\n", response);

        close(clientSocket);
    }

    return 0;
}
