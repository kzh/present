#pragma once

#ifndef HTTPSERVER
#define HTTPSERVER

#include <netdb.h>
#include <pthread.h>

struct Server {
    struct sockaddr_in addr;
    int socket;
};

void init_server(struct Server *server); 
#endif
