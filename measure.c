#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <time.h>


#define SERVER_PORT 5060
#define SERVER_IP_ADDRESS "127.0.0.1"


int main() {

/**
 * TCP Socket opening
*/
    // Create socket descriptor (open TCP socket)
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    // Create connection with measure
    struct sockaddr_in serverAddress;

    memset(&serverAddress, 0, sizeof(serverAddress)); //Delete n first chars of string  s, and put instead c

    serverAddress.sin_family = AF_INET; // Address family, AF_INET unsigned (AF_INET for IPv4)
    serverAddress.sin_port = htons(SERVER_PORT); // Port number (htons for conversion: sin port to Network Byte Order  )
    // Conversion of IPv4 address to Binary/Net representation by using inet_pton()
    int rval = inet_pton(AF_INET, (const char *) SERVER_IP_ADDRESS, &serverAddress.sin_addr);
    if (rval <= 0) {
        printf("inet_pton() failed");
        return -1;
    }
/**
* Create connection with server (measure)
*/
    if (connect(sock, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1) {
        printf("connect() failed");// If the connection did not created the function will return -1
        return -1;
    }
    else
    {
        printf("connected to server\n");
    }


    clock_t begin = clock();
    /*
     * Get data from the server
    */
    char *message = (char *) malloc(1016000 * sizeof(char));// Get data from the client
//    char message[1016000];
    uint receiveFlag = recv(sock, message, 1016000, 0);

    if (receiveFlag == -1) {
        close(sock);
        return -1;
    }
    printf("file successfully received\n");

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Time taken: %f", time_spent);
    free(message);
    return 0;
}