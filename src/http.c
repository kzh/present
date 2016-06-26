#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http.h"

int parse_header(struct Header *header, char *str) {
    if (strstr(str, ":") == NULL) {
        return 0;
    }

    char *save;
    header->name  = strtok_r(str, ":", &save);
    header->value = strtok_r(NULL, "", &save) + 1;
    return 1;
} 

void insert_header(struct HttpRequest *req, struct Header *header) {
    header->next = (struct Header*) malloc(sizeof(struct Header));
    if (req->headers_count == 0) {
        memcpy(req->headers, header, sizeof(struct Header));
    } else {
        struct Header* tmp = req->headers;
        for (int i = 0; i != req->headers_count; i++) {
            tmp = tmp->next;
        }
        memcpy(tmp, header, sizeof(struct Header));
    }
    req->headers_count++;
}

void parse_http_req(struct HttpRequest *req, char *str) {
    req->raw = (char*) malloc(strlen(str) + 1);
    strcpy(req->raw, str);
    req->headers_count = 0;
    req->headers       = (struct Header*) malloc(sizeof(struct Header));

    char *save;
    req->method  = strtok_r(req->raw, " ", &save);
    req->path    = strtok_r(NULL, " ", &save);
    req->version = strtok_r(NULL, "\n\r", &save);

    char *token;
    while ((token = strtok_r(NULL, "\n\r", &save)) != NULL) {
        struct Header header;
        if (!parse_header(&header, token)) {
            break;
        }

        insert_header(req, &header);
    }
}
