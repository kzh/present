#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "http.h"
#include "server.h"

#define HTTPREQUESTSIZE 1024 
#define SERVER_BACKLOG  10
#define SERVER_PORT     80 

void* handle_request(void* client);

void init_server(struct Server *server) {
    server->socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server->socket == -1) {
        perror("Socket creation failed");
    }

    int on = 1;
    if (setsockopt(server->socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) {
        perror("Socket configuring failed");
    }

    memset(&(server->addr), 0, sizeof(server->addr));
    server->addr.sin_family      = AF_INET;
    server->addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server->addr.sin_port        = htons(SERVER_PORT);

    if (bind(server->socket, (struct sockaddr*) &(server->addr), sizeof(struct sockaddr)) == -1) {
        perror("Socket binding failed");
    }

    if (listen(server->socket, SERVER_BACKLOG) == -1) {
        perror("Socket listen failed");
        exit(0);
    }

    pthread_t pid;
    struct sockaddr_in6 cli_addr;
    int client;
    while (1) {
        socklen_t clilen = sizeof(cli_addr); 
        client = accept(server->socket, (struct sockaddr*) &cli_addr, &clilen);
        if (client == -1) {
            sleep(1);
            continue;
        }

        int* cli_socket = (int*) malloc(sizeof(cli_socket));
        *cli_socket = client;
        //pthread_create(&pid, NULL, handle_request, cli_socket);
        handle_request(cli_socket);
    }
}

void* handle_request(void* arg) {
    printf("Accepted \n");
    int client = *((int*) arg);

    int buffer_in_length;
    char buffer_in[HTTPREQUESTSIZE];
    if ((buffer_in_length = recv(client, buffer_in, sizeof(buffer_in) - 1, 0)) == 0) {
        perror("Receiving failed");
    } else {
        buffer_in[buffer_in_length] = '\0';
        printf("%s \n", buffer_in);

        struct HttpRequest req;
        parse_http_req(&req, buffer_in);

        char* reply = "hello world";
        send(client, reply, strlen(reply), 0);
    }

    free(arg);
    close(client);
    pthread_detach(pthread_self());
    return NULL;
}
