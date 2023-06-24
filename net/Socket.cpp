#include <string.h>

#include "Socket.h"
#include "Serializable.h"
#include "Message.h"

using namespace std;
Socket::Socket(const char * address, const char * port):sd(-1)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = 0;
    struct addrinfo* res;

    int sc = getaddrinfo(address, port, &hints, &res);
    if(sc != 0){
        cerr << "[getaddrinfo]: " << gai_strerror(sc) << "\n";
        throw ("ERR - gataddrinfo: " + string(gai_strerror(sc)));
    }

    sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if(sd < 0){
        cout << "ERR - could not create socket. Error code " << errno << "\n";
        freeaddrinfo(res);
        throw "ERR - could not open socket";
    }

    sa_len = res->ai_addrlen;
    sa = *res->ai_addr;
}

int Socket::recv(Serializable &obj, Socket ** sock)
{
    struct sockaddr sa;
    socklen_t sa_len = sizeof(struct sockaddr);

    char buffer[Message::MAX_SIZE];

    ssize_t bytes = ::recvfrom(sd, buffer, Message::MAX_SIZE, MSG_DONTWAIT, &sa, &sa_len);
    if ( bytes <= 0 )
    {
        return -1;
    }

    if ( sock != 0 )
    {
        (*sock) = new Socket(&sa, sa_len);
    }

    obj.from_bin(buffer);

    return bytes;
}

int Socket::send(Serializable& obj, const Socket& sock)
{
    //Serialize and send message to socket
    obj.to_bin();

    std::cout << "Datos a enviar: " << obj.data() << "\n";

    ssize_t bytes = sendto(sd, obj.data(), obj.size(), 0, &sock.sa, sock.sa_len);

    if(bytes <= 0){
        return -1;
    }

    return bytes;
}

bool operator== (const Socket &s1, const Socket &s2)
{
    //Compare sockets based on their sockaddr structures.
    sockaddr_in  * sin1 = (sockaddr_in *) &s1.sa;
    sockaddr_in  * sin2 = (sockaddr_in *) &s2.sa;

    bool ret = sin1->sin_family == sin2->sin_family && sin1->sin_addr.s_addr == sin2->sin_addr.s_addr && sin1->sin_port == sin2->sin_port;
    return ret;
}

std::ostream& operator<<(std::ostream& os, const Socket& s)
{
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    getnameinfo((struct sockaddr *) &(s.sa), s.sa_len, host, NI_MAXHOST, serv,
                NI_MAXSERV, NI_NUMERICHOST);

    os << host << ":" << serv;

    return os;
};