#include <unistd.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

#define SERVER_PORT 5060  //The port that the server listens


int handleSendErr(long code, ulong length, uint bytesSent) {
    if (-1 == code) {
        printf("send() failed\n");
        return 0;
    } else if (0 == code) {
        printf("peer has closed the TCP connection prior to send().\n");
        return 0;
    } else if (length > bytesSent) {
        printf("sent only %lu bytes from the required %d.\n", length, bytesSent);
        return 0;
    } else {
        printf("message was successfully sent .\n");
        return 1;
    }
}


int main() {
    signal(SIGPIPE, SIG_IGN); // on linux to prevent crash on closing socket
/*
 Open socket TCP
 */
    // Open the listening (server) socket
    int listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listeningSocket == -1) {
        printf("Could not create listening socket");
        return -1;
    }
/*
Listen to incoming connections
 */
    // Reuse the address if the server socket on was closed
    // and remains for 45 seconds in TIME-WAIT state till the final removal.
    int enableReuse = 1;
    if (setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int)) < 0) {
        printf("setsockopt() failed");
        return -1;
    }

    // "sockaddr_in" is the "derived" from sockaddr struct, used for IPv4 communication.
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));//Delete n first chars of string  s, and put instead c

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(SERVER_PORT);  //network order

    // Bind the socket to the port with any IP at this port
    if (bind(listeningSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1) //to let us reuse port
    {
        printf("Bind failed");
        return -1;
    }

    printf("Bind() success\n");

    //Server change status to 'waiting'
    if (listen(listeningSocket, 500) < 0) {
        printf("listen() failed");
        return -1;
    }

    printf("Waiting for incoming TCP-connections...\n");
    //Accept and incoming connection
    struct sockaddr_in clientAddress;  //
    socklen_t clientAddressLen = sizeof(clientAddress);

    while (1) {
        memset(&clientAddress, 0, sizeof(clientAddress));
        clientAddressLen = sizeof(clientAddress);

        int clientSocket = accept(listeningSocket, (struct sockaddr *) &clientAddress, &clientAddressLen);
        if (clientSocket == -1) {
            printf("listen failed");
            close(listeningSocket);
            return -1;
        }
        printf("A new client connection accepted\n");//pop a request from the request queue


        /*
        * Open and read the file
        */
        FILE *fptr;
        fptr = fopen("/home/appeldaniel/CLionProjects/untitled/dummy.txt", "r");
        if (fptr == NULL) {
            printf("Error! opening file");
            return -1;
        }
        printf("File Opened\n");
        fseek(fptr, 0, SEEK_END);
        uint length = ftell(fptr);
        fseek(fptr, 0, SEEK_SET);
        char message[1048577] = "";
//        char* message = (char *) malloc(length);
        if (message) {
            fread(message, 1, 1048577, fptr);
        }
        fclose(fptr);


        printf("File Closed\n");

        /*
        * Send the file
        */
//        long bytesSent = send(clientSocket, message, strlen(message) + 1, 0);
//        if (!(handleSendErr(bytesSent, strlen(message) + 1, bytesSent))) return -1;
//        printf("File 1 sent\n");

        char buf[256];
        strcpy(buf, "reno");
        socklen_t len = strlen(buf);
        if (setsockopt(listeningSocket, IPPROTO_TCP, TCP_CONGESTION, buf, len) != 0) {
            perror("setsockopt");
            return -1;
        }

        long bytesSent = send(clientSocket, message, strlen(message) + 1, 0);
        if (!(handleSendErr(bytesSent, strlen(message) + 1, bytesSent))) return -1;
        printf("File 2 sent\n");

        close(clientSocket);
    }

    //Close connection

    close(listeningSocket);
    return 0;
}
