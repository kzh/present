#include <netdb.h>

struct Server {
    struct sockaddr_in serv_addr;
    int socket;
};

void init_server(struct Server *server); 
