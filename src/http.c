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
    header->value = strtok_r(NULL, "", &save);
    return 1;
}

void insert_header(struct HttpRequest *req, struct Header *header) {
    header->next = (struct Header*) calloc(1, sizeof(struct Header));
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

void prepare_str(char *str) {
    int length = strlen(str);
    for (int i = 0; i != length; i++) {
        if (str[i] != ':' || str[i + 1] != ' ') {
            continue;
        }

        char *dest = str + (++i);
        while (i < length && (str[++i] == ' ' || str[i] == '\t')); 

        char *move = str + i;
        if (dest != move) {
            memmove(dest, move, strlen(move) + 1);
        }

        i = dest - str;

        while (1) {
            while (i < length && (str[++i] != '\r' && str[i] != '\n'));
            dest = str + i;
            if (str[i + 1] == ' ' || str[i + 1] == '\t') {
                while (i < length && (str[++i] == ' ' || str[i] == '\t'));
                memmove(dest, str + i, strlen(str + i) + 1);
            } else {
                break;
            }
        }
    }
}

void parse_http_req(struct HttpRequest *req, char *str) {
    prepare_str(str);
    req->raw = strdup(str);
    req->headers_count = 0;
    req->headers       = (struct Header*) calloc(1, sizeof(struct Header));

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
    printf("%s\n", get_header_from(req, "Host"));
}

char* get_header_from(struct HttpRequest *req, char *search) {
    if (req->headers_count == 0) {
        return NULL;
    }

    struct Header *header = req->headers;
    while (header->next != NULL) {
        if (strcmp(header->name, search) == 0) {
            return header->value;
        }

        header = header->next;
    }

    return NULL;
}

void free_headers(struct Header *headers) {
    if (headers->next != NULL) {
        free_headers(headers->next);
    }

    free(headers);
}

void free_http_req(struct HttpRequest *req) {
    free_headers(req->headers);
    free(req->raw);
}
