#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int isIpAddressValid(const char *ipAddress) {
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
    return result != 0;
}

int isPortValid(int port) {
    return (port > 0 && port <= 65535);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Please provide the server IP address and port as command-line arguments.\n");
        return 1;
    }

    char *serverIp = argv[1];
    int serverPort = atoi(argv[2]);

    if (!isIpAddressValid(serverIp)) {
        printf("Invalid server IP address.\n");
        return 1;
    }

    if (!isPortValid(serverPort)) {
        printf("Invalid server port.\n");
        return 1;
    }

    int clientSocket;
    struct sockaddr_in serverAddress;
    char buffer[1024];

    // Create client socket
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set server address configuration
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(serverIp);
    serverAddress.sin_port = htons(serverPort);

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server.\n");

    // Send and receive data
    while (1) {
        printf("Enter a message: ");
        fgets(buffer, 1024, stdin);

        // Send data to the server
        if (send(clientSocket, buffer, strlen(buffer), 0) < 0) {
            perror("Send failed");
            exit(EXIT_FAILURE);
        }

        // Receive data from the server
        memset(buffer, 0, sizeof(buffer));
        if (recv(clientSocket, buffer, sizeof(buffer), 0) < 0) {
            perror("Receive failed");
            exit(EXIT_FAILURE);
        }

        printf("Server response: %s\n", buffer);
    }

    // Close the client socket
    close(clientSocket);

    return 0;
}
