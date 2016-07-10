#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "http.h"
#include "server.h"

#define HTTPREQUESTSIZE 1024 
#define SERVER_BACKLOG  10
#define SERVER_PORT     80 

struct client {
    struct server *server;
    int socket;
};

char* identify(struct server *server, char* p) {
    char* path = malloc(sizeof(p) + 1);
    strcpy(path, p);

    for (struct route *route = server->routes; route != NULL; route = route->next) {
        if (strncmp(route->route, path, strlen(route->route)) != 0) {
            continue;
        }

        char* tmp = (char*) malloc(strlen(route->to) + strlen(path + strlen(route->route)) + 1);
        strcpy(tmp, route->to);
        strcat(tmp, "/");
        strcat(tmp, path + strlen(route->route));

        free(path);
        path = tmp;
        break;
    }

    if (path[strlen(path) - 1] == '/') {
        const char* alts[] = {"index.html", "index.tmpl"};
        for (int i = 0; i != sizeof(alts) / sizeof(alts[0]); i++) {
            char *tmp = (char*) malloc(strlen(path) + strlen(alts[i]));
            strcpy(tmp, path);
            strcat(tmp, alts[i]);

            if (access(tmp, F_OK) == -1) {
                free(tmp);
                continue;
            }

            free(path);
            path = tmp;
        }
    }

    return path;
}

void respond(struct client *client, struct http_message *msg) {
    char* path = identify(client->server, msg->info[REQUEST_PATH]);
    printf("Path: %s\n", path);

    FILE *f;
    f = fopen(path, "r");

    if (f == NULL) {
        /*
        char *moved = NULL;
        if (path[strlen(path) - 1] != '/') {
            char tmp[strlen(path) + 1];
            strcpy(tmp, path);
            strcat(tmp, "/");

            moved = identify(tmp);
            if (strcmp(moved, tmp) != 0) {

            } else {
                free(moved);
                moved = NULL;
            }
        }

        if (moved == NULL) {

        }
        */
    } else {
        fseek(f, 0, SEEK_END);
        long size = ftell(f);
        fseek(f, 0, SEEK_SET);

        char reply[size];
        fread(reply, size, 1, f);

        send(client->socket, reply, size, 0);
    }

    fclose(f);
    free(path);
}

void* handle_request(void* arg) {
    struct client *client = (struct client*) arg;

    int buffer_in_length;
    char buffer_in[HTTPREQUESTSIZE];
    if ((buffer_in_length = recv(client->socket, buffer_in, sizeof(buffer_in) - 1, 0)) == 0) {
        perror("Receiving failed");
    } else {
        buffer_in[buffer_in_length] = '\0';
        printf("%s \n", buffer_in);

        struct http_message msg;
        parse_http_message(&msg, buffer_in);
        respond(client, &msg);
        free_http_message(&msg);
    }

    free(arg);
    close(client->socket);
    pthread_detach(pthread_self());
    return NULL;
}

void init_server(struct server *server) {
    memset(server, 0, sizeof(struct server));
}

void run_server(struct server *server) {
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
    int cli_socket;
    while (1) {
        socklen_t clilen = sizeof(cli_addr);
        cli_socket = accept(server->socket, (struct sockaddr*) &cli_addr, &clilen);
        if (cli_socket == -1) {
            sleep(1);
            continue;
        }

        struct client *client = (struct client*) malloc(sizeof(struct client));
        *client = (struct client) {server, cli_socket};
        pthread_create(&pid, NULL, handle_request, client);
    }
}

void serve(struct server *server, char *route, char *to) {
    struct route *r = (struct route*) malloc(sizeof(struct route));
    r->route = route;
    r->to    = to;
    r->next  = NULL;

    if (server->routes == NULL) {
        server->routes = r;
    } else {
        struct route *tmp;
        for (; tmp->next != NULL; tmp = tmp->next);
        tmp->next = r;
    }
}
