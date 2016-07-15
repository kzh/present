#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "http.h"

int main() {
    char raw[] = "GET / HTTP/1.1\r\n"
                "Host: localhost\r\n"
                "From: kevtehpro35(at)gmail.com\r\n"
                "\r\n"
                "Hola como esta";

    printf("=========== RAW ===========\n%s\n===========================\n", raw);

    struct http_message msg;
    parse_http_message(&msg, raw); 

    printf("Method: %s\n", msg.info[REQUEST_METHOD]);
    printf("Path: %s\n", msg.info[REQUEST_PATH]);
    printf("Verson: %s\n", msg.info[REQUEST_VERSION]);

    char *host = get_header_from(&msg, "Host");
    printf("The host is %s\n", host);
     char *from = get_header_from(&msg, "From");
    printf("The from is %s\n", from);

    printf("Message:\n");
    printf("%s\n", msg.message);

    char *str = encode_http_message(&msg);
    printf("=========== NEW ===========\n%s\n===========================\n", str);

    free(str);
    free_http_message(&msg);
    return 0;
}
