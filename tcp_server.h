#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <sys/socket.h>
#include <netinet/in.h>

#include <QDebug>

typedef unsigned short int uint2_t;

#define SOCKET_FAMILY AF_INET
#define SOCKET_TYPE   SOCK_STREAM
#define SOCKET_PROTO  IPPROTO_TCP

class tcp_server
{
public:
    tcp_server(struct sockaddr*, int listen_queue, int send_size, int recv_size);
    virtual ~tcp_server() = default;
    
    void start();
private:
    int srv_socket;
    int clnt_socket;
    int send_size;
    int recv_size;
    
    struct sockaddr* srv_addr;
    struct sockaddr* clnt_addr = new struct sockaddr;
};

#endif // TCP_SERVER_H
