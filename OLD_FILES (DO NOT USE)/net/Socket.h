#ifndef SOCKET_H_
#define SOCKET_H_

#define MAX_CLIENTS 12

// Function to create and initialize the server socket
int createServerSocket(int port);

// Function to accept incoming client connections
int acceptClientConnection(int serverSocket);

// Function to handle client requests and data
void handleClientRequests(int serverSocket, int clientSockets[], int maxClients);

#endif /* SOCKET_H_ */
