#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include <pthread.h>

#include "server.h"

#define SERVER_BACKLOG 10
#define SERVER_PORT    80

void* handle_request(void* client);

void init_server(struct Server *server) {
    server->socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server->socket == -1) {
        perror("Socket creation failed");
    }

    int on = 1;
    if (setsockopt(server->socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) == -1) {
        perror("Socket configuring failed");
    }

    memset(&(server->serv_addr), 0, sizeof(struct sockaddr_in));
    server->serv_addr.sin_family      = AF_INET;
    server->serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server->serv_addr.sin_port        = htons(SERVER_PORT);

    if (bind(server->socket, (struct sockaddr*) &(server->serv_addr), sizeof(struct sockaddr)) == -1) {
        perror("Socket binding failed");
    }

    if (listen(server->socket, SERVER_BACKLOG) == -1) {
        perror("Socket listen failed");
        exit(0);
    }

    char* reply = "Hello \n";
    struct sockaddr_in6 cli_addr;
    int client;
    while (1) {
        socklen_t clilen = sizeof(struct sockaddr_in6); 
        client = accept(server->socket, (struct sockaddr*) &cli_addr, &clilen);
        write(client, reply, strlen(reply)); 
        close(client);
        sleep(1);
    }
}

/*
void* handle_request(void* client) {
}
*/
