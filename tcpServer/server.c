#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <math.h>

#define TEXT_SIZE 1024
#define P_NUMBER 131
#define G_NUMBER 29
#define PRIVATE_KEY 4

int keysGeneration(int gen,int private,int public){
    return private==1 ? gen: (((int)pow(gen, private)) % public);
}

int main(int argc, char const* argv[]) {
    int echoStatus = 0;
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddress, clientAddress;
    char textHandler[TEXT_SIZE];
    char exitCommand[] = "exit";
    char aboutMessage[] = "\nThis service was created as 1 task in the subject\n"
                          "Security of information and communication technologies.\n"
                          "Created by Artem Petrenko .\n";
    char startMessage[] ="\n"
                          "    // ----------------------------------------------------------\n"
                          "    // help - server commands guide\n"
                          "    // about - project some words about this project\n"
                          "    // echo-mode - echo answers from server on/off \n"
                          "    // cipher - some words about key-exchange at this app\n"
                          "    // exit - close connection\n"
                          "    // ----------------------------------------------------------\n";
    char echoOnMessage[] = "Echo mode is on\n";
    char echoOffMessage[] = "Echo mode is off\n";
    char errorMessage[] = "\nWrong command - put help to get server commands\n";
    char chipherAbout[] = "\nThis application is supported by Diffie-Hellman key-exchange algorithm. \n"
                          "The Diffie-Hellman algorithm is being used to establish a shared secret \n"
                          "that can be used for secret communications while exchanging data over a \n"
                          "public network using the elliptic curve to generate points and get the \n"
                          "secret key using the parameters.  \n";
    char keyTransfer[20];

    sprintf(keyTransfer, "%d", keysGeneration(G_NUMBER,PRIVATE_KEY,P_NUMBER));


    serverSocket = socket(AF_INET, SOCK_STREAM, 0) ;

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8080);

    bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    printf("Server is listening ...\n");
    listen(serverSocket, 1);

    int clientAddress_len = sizeof(clientAddress);

    while (1) {
        ssize_t bytesHandled;

        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddress_len);
        printf("Connection confirmed ...\n");
        send(clientSocket, startMessage,sizeof startMessage, 0);


        recv(clientSocket, textHandler, TEXT_SIZE, 0);
        send(clientSocket,keyTransfer , sizeof keyTransfer, 0);

        memset(keyTransfer,'\0',sizeof keyTransfer);

        printf("Client key recieved...\n");

        printf("Client key validating Diffie-Hellman key exchange...\n");
        printf("Handled client key...\n", textHandler);
        int keyClient = atoi(textHandler);

        printf("Generating secret key...\n");
        int secretKeyServer = keysGeneration(keyClient,PRIVATE_KEY,P_NUMBER);

        sprintf(keyTransfer,"%d",secretKeyServer);
        printf("Comparing keys...\n");

        send(clientSocket,keyTransfer , sizeof keyTransfer, 0);
        recv(clientSocket, textHandler, TEXT_SIZE, 0);

        if (atoi(textHandler)==secretKeyServer){

            printf("Permission accepted...\n");
            while ((bytesHandled = recv(clientSocket, textHandler, TEXT_SIZE, 0)) > 0) {

                textHandler[bytesHandled] = '\0';

                if (strncmp(textHandler, "help",4) == 0){
                    send(clientSocket, startMessage, sizeof startMessage, 0);
                    printf("Client Request: help guide\n");
                }
                else if (strncmp(textHandler, "echo-mode",9) == 0){
                    if (echoStatus == 0){
                        send(clientSocket, echoOnMessage, sizeof echoOnMessage, 0);
                        echoStatus = 1;
                        printf("Client Request echo status on\n");
                    }else{
                        send(clientSocket, echoOffMessage, sizeof echoOffMessage, 0);
                        echoStatus = 0;
                        printf("Client Request echo status off\n");
                    }
                    printf("Client Request: help guide\n");
                }
                else if (strncmp(textHandler, "about",5) == 0){
                    send(clientSocket, aboutMessage, sizeof aboutMessage, 0);
                    printf("Client Request: about project information\n");
                }else if(strncmp(textHandler, "cipher",6) == 0){
                    send(clientSocket, chipherAbout, sizeof chipherAbout, 0);
                    printf("Client Request: about cipher in app information\n");
                }
                else if (echoStatus != 0){
                    printf("Client Request: %s", textHandler);
                    send(clientSocket, textHandler, bytesHandled, 0);
                }else{
                    printf("Client Request: wrong command\n");
                    send(clientSocket, errorMessage, sizeof errorMessage, 0);
                }
            }
        }else{
            printf("Permission denied...\n");
            break;
        }


        if (bytesHandled == 0 || strcmp(textHandler, exitCommand) == 0) {
            break;
        }
    }

    close(clientSocket);
    printf("Client disconnected.\n");

    close(serverSocket);
    printf("Server disconnected.\n");

    return 0;
}