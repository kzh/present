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

void insert_header(struct http_message *msg, struct header *header) {
    header->next = (struct header*) calloc(1, sizeof(struct header));
    if (msg->headers_count == 0) {
        memcpy(msg->headers, header, sizeof(struct header));
    } else {
        struct header *tmp = msg->headers;
        for (int i = 0; i != msg->headers_count; i++) {
            tmp = tmp->next;
        }
        memcpy(tmp, header, sizeof(struct header));
    }
    msg->headers_count++;
}

// Condenses raw http message strings
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

void parse_http_message(struct http_message *msg, char *str) {
    prepare_str(str);

    msg->headers_count = 0;
    msg->headers       = (struct header*) calloc(1, sizeof(struct header));

    char *save;
    msg->info[REQUEST_METHOD]  = strtok_r(str, " ", &save);
    msg->info[REQUEST_PATH]    = strtok_r(NULL, " ", &save);
    msg->info[REQUEST_VERSION] = strtok_r(NULL, "\n\r", &save);

    char *token;
    while ((token = strtok_r(NULL, "\n\r", &save)) != NULL) {
        struct header header;
        if (!parse_header(&header, token)) {
            break;
        }

        insert_header(msg, &header);
    }
}

char* get_header_from(struct http_message *msg, char *search) {
    if (msg->headers_count == 0) {
        return NULL;
    }

    struct header *header = msg->headers;
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

void free_http_message(struct http_message *msg) {
    free_headers(msg->headers);
}
