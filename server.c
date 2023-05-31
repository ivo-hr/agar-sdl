#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#define PORT 12345
#define MAX_CLIENTS 12

int main() {
    int serverSocket, newSocket, clientSockets[MAX_CLIENTS];
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t clientAddressLength;
    char buffer[1024];
    int maxClients = MAX_CLIENTS;
    int activity, i, valread, sd, maxSocket;

    // Create server socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set server address configuration
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    // Bind the server socket to a specific address and port
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(serverSocket, 3) < 0) {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }

    // Accept incoming connections and handle client requests
    while (1) {
        // Clear the socket set and add the server socket to it
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(serverSocket, &readSet);
        maxSocket = serverSocket;

        // Add child sockets to the set
        for (i = 0; i < maxClients; i++) {
            sd = clientSockets[i];
            if (sd > 0)
                FD_SET(sd, &readSet);
            if (sd > maxSocket)
                maxSocket = sd;
        }

        // Wait for activity on one of the sockets
        activity = select(maxSocket + 1, &readSet, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR)) {
            perror("Select error");
        }

        // Handle new connection
        if (FD_ISSET(serverSocket, &readSet)) {
            if ((newSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength)) < 0) {
                perror("Accepting connection failed");
                exit(EXIT_FAILURE);
            }

            // Add new connection to the client sockets array
            for (i = 0; i < maxClients; i++) {
                if (clientSockets[i] == 0) {
                    clientSockets[i] = newSocket;
                    break;
                }
            }
        }

        // Handle client requests and data
        for (i = 0; i < maxClients; i++) {
            sd = clientSockets[i];
            if (FD_ISSET(sd, &readSet)) {
                // Handle data received from client
                if ((valread = read(sd, buffer, 1024)) == 0) {
                    // Client disconnected
                    getpeername(sd, (struct sockaddr *)&clientAddress, &clientAddressLength);
                    printf("Client disconnected: IP %s, Port %d\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));
                    close(sd);
                    clientSockets[i] = 0;
                } else {
                    // Process client request and send response
                    buffer[valread] = '\0';
                    printf("Received from client: %s\n", buffer);

                    // TODO: Process the client request and prepare the response

                    // Send response to the client
                    char response[1024] = "Hello from server";
                    send(sd, response, strlen(response), 0);
                }
            }
        }
    }

    return 0;
}
