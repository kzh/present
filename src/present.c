#include "server.h"

int main() {
    struct server server;
    init_server(&server);
    serve(&server, "/", "public");
    run_server(&server);

    return 0;
}
