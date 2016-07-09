#pragma once

#ifndef HTTPSERVER
#define HTTPSERVER

#include <netdb.h>

struct route {
    char *route, *to;

    struct route *next;
};

struct server {
    struct sockaddr_in addr;
    int socket;

    struct route *routes;
};

void init_server(struct server *server); 
void run_server(struct server *server); 
void serve(struct server *server, char *path, char *to);
#endif
