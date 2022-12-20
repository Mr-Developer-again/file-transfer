#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "tcp_server.h"

tcp_server::tcp_server(struct sockaddr* srv_addr, int listen_queue, int send_size, int recv_size)
{
    this->send_size = send_size;
    this->recv_size = recv_size;
    this->srv_addr = srv_addr;

    if ((this->srv_socket = socket(SOCKET_FAMILY, SOCKET_TYPE, SOCKET_PROTO)) < 0)
    {
        perror("couldn't create server's socket");
        exit(EXIT_FAILURE);
    }

    if (bind(this->srv_socket, this->srv_addr, sizeof(*this->srv_addr)) < 0)
    {
        perror("binding failed");
        exit(EXIT_FAILURE);
    }
    
    if (listen(this->srv_socket, listen_queue) < 0)
    {
        perror("listening failed");
        exit(EXIT_FAILURE);
    }
}

void tcp_server::start()
{
    ;
}