#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <math.h>


#define TEXT_SIZE 1024
#define P_NUMBER 131
#define G_NUMBER 29
#define PRIVATE_KEY 2

int keysGeneration(int gen,int private,int public){
    return private==1 ? gen: (((int)pow(gen, private)) % public);
}

int main() {
    int clientSocket;
    struct sockaddr_in serverAddress;
    char inputText[TEXT_SIZE];
    char exitCommand[] = "exit";
    char keyTransfer[20];

    sprintf(keyTransfer, "%d", keysGeneration(G_NUMBER,PRIVATE_KEY,P_NUMBER));


    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8080);

    // Connect to the server
    connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    printf("Connected to the server.\n");
    recv(clientSocket, inputText, TEXT_SIZE, 0);
    printf("%s\n", inputText);

    send(clientSocket,keyTransfer , sizeof keyTransfer, 0);

    recv(clientSocket, inputText, TEXT_SIZE, 0);
    printf("Server key recieved...\n");
    memset(keyTransfer,'\0',sizeof keyTransfer);

    printf("Server key validating Diffie-Hellman key exchange...\n");
    printf("Handled Server key...\n", inputText);
    int keyClient = atoi(inputText);
    printf("Generating secret key... \n");
    int secretKeyClient = keysGeneration(keyClient,PRIVATE_KEY,P_NUMBER);
    sprintf(keyTransfer,"%d",secretKeyClient);
    send(clientSocket,keyTransfer , sizeof keyTransfer, 0);
    recv(clientSocket, inputText, TEXT_SIZE, 0);
    printf("Comparing keys...\n");

    if (atoi(inputText)==secretKeyClient) {

        printf("Permission accepted...\n");
        while (1) {

            printf("Input message: ");
            fgets(inputText, TEXT_SIZE, stdin);
            send(clientSocket, inputText, sizeof(inputText), 0);

            if (strncmp(inputText, exitCommand, sizeof(exitCommand) - 1) == 0) {
                printf("Exiting client...\n");
                break;
            }

            ssize_t bytes_received = recv(clientSocket, inputText, TEXT_SIZE, 0);
            if (bytes_received > 0) {
                inputText[bytes_received] = '\0';
                printf("Server response: %s", inputText);
            } else {
                printf("Server closed the connection.\n");
                break;
            }
        }
    }else{
        printf("Permission denied...");
    }

    close(clientSocket);
    printf("Client disconnected.\n");


    return 0;
}