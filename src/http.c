#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http.h"

int parse_header(struct header *header, char *str) {
    if (strstr(str, ":") == NULL) {
        return 0;
    }

    char *save;
    header->name  = strtok_r(str, ":", &save);
    header->value = strtok_r(NULL, "", &save);
    return 1;
}

void insert_header(struct http_request *req, struct header *header) {
    header->next = (struct header*) calloc(1, sizeof(struct header));
    if (req->headers_count == 0) {
        memcpy(req->headers, header, sizeof(struct header));
    } else {
        struct header *tmp = req->headers;
        for (int i = 0; i != req->headers_count; i++) {
            tmp = tmp->next;
        }
        memcpy(tmp, header, sizeof(struct header));
    }
    req->headers_count++;
}

// Condense raw http request strings
// - Removes spaces/tabs between : and header value
// - Combines multilined header values into one line 
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

void parse_http_req(struct http_request *req, char *str) {
    prepare_str(str);

    char* tmp = malloc(strlen(str) + 1);
    strcpy(tmp, str);

    req->raw = tmp;
    req->headers_count = 0;
    req->headers       = (struct header*) calloc(1, sizeof(struct header));

    char *save;
    req->method  = strtok_r(req->raw, " ", &save);
    req->path    = strtok_r(NULL, " ", &save);
    req->version = strtok_r(NULL, "\n\r", &save);

    char *token;
    while ((token = strtok_r(NULL, "\n\r", &save)) != NULL) {
        struct header header;
        if (!parse_header(&header, token)) {
            break;
        }

        insert_header(req, &header);
    }
}

char* get_header_from(struct http_request *req, char *search) {
    if (req->headers_count == 0) {
        return NULL;
    }

    struct header *header = req->headers;
    while (header->next != NULL) {
        if (strcmp(header->name, search) == 0) {
            return header->value;
        }

        header = header->next;
    }

    return NULL;
}

void free_headers(struct header *headers) {
    if (headers->next != NULL) {
        free_headers(headers->next);
    }

    free(headers);
}

void free_http_req(struct http_request *req) {
    free_headers(req->headers);
    free(req->raw);
}
