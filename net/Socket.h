#ifndef SOCKET_H_
#define SOCKET_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include <iostream>
#include <stdexcept>

#include <ostream>

// Class forward declarations
class Socket;
class Serializable;


// Compare sockets based on their sockaddr structures.
bool operator== (const Socket &s1, const Socket &s2);


// Print the socket address in an understandable way.
std::ostream& operator<<(std::ostream& os, const Socket& dt);


// This class represents the local endpoint of a UDP connection. Is able to initialize
// a socket and the binary description of the endpoint.
class Socket
{
public:

    //Max theoretical size of a UDP message
    static const int32_t MAX_MESSAGE_SIZE = 32768;


    //Socket builder with address and port.
    Socket(const char * address, const char * port);


    //Socket builder with a sockaddr structure.
    Socket(struct sockaddr * _sa, socklen_t _sa_len):sd(-1), sa(*_sa),
        sa_len(_sa_len){};

    virtual ~Socket(){};


    // Receive a message (obj) from the network, recognising the sender. 
    //Returns 0 if successfull or -1 if there was an error (connection closed).
    int recv(Serializable &obj, Socket ** sock);

    // Receive a message (obj) from the network, without caring about the sender.
    int recv(Serializable &obj)
    {
        Socket * s = 0;

        return recv(obj, (&s));
    }

    int getDescriptor()
    {
        return sd;
    }
    
    // Send a message (obj) to the given host (sock). Returns 0 if successfull or -1
    // if there was an error.
    int send(Serializable& obj, const Socket& sock);

    
    // Bind the socket to the local address. Returns 0 if successfull or -1 if there
    // was an error.
    int bind()
    {
        return ::bind(sd, (const struct sockaddr *) &sa, sa_len);
    }

    friend std::ostream& operator<<(std::ostream& os, const Socket& dt);

    friend bool operator== (const Socket &s1, const Socket &s2);

protected:

    // Socket descriptor
    int sd;

    // Binary representation of the endpoint address
    struct sockaddr sa;
    socklen_t       sa_len;
};

#endif