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

    char* value = strtok_r(NULL, "", &save);
    header->value = malloc(strlen(value));
    strcpy(header->value, value);
    return 1;
}

void insert_header(struct http_message *msg, struct header *header) {
    header->next = (struct header*) calloc(1, sizeof(struct header));
    if (msg->headers_count == 0) {
        msg->headers = (struct header*) calloc(1, sizeof(struct header));
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

extern inline void concat(char **str, char* add) {
    char *tmp = (char*) malloc(strlen(*str) + strlen(add));
    strcpy(tmp, *str);
    strcat(tmp, add);

    free(*str);
    *str = tmp;
}

char* encode_http_message(struct http_message *msg) {
    char *raw = (char*) malloc(1);
    strcpy(raw, "");

    for (int i = 0; i != sizeof(msg->info) / sizeof(msg->info[0]); i++) {
        if (msg->info[i] == NULL) {
            break;
        }

        concat(&raw, msg->info[i]);
        if (i != sizeof(msg->info) / sizeof(msg->info[0]) - 1) {
            concat(&raw, " ");
        }
    }
    concat(&raw, "\r\n");

    if (msg->message != NULL) {
        concat(&raw, "Content-Length: "); 

        char size[100];
        sprintf(size, "%ld", strlen(msg->message));
        concat(&raw, size); 
        concat(&raw, "\r\n");
    } 

    if (msg->headers_count != 0) {
        for (struct header *header = msg->headers; header->next != NULL; header = header->next) {
            concat(&raw, header->name);
            concat(&raw, ": ");
            concat(&raw, header->value);
            concat(&raw, "\r\n");
        }
    }
    concat(&raw, "\r\n");

    if (msg->message != NULL) {
        concat(&raw, msg->message);
    }

    return raw;
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

    free(headers->value);
    free(headers);
}

void free_http_message(struct http_message *msg) {
    if (msg->message != NULL) {
        free(msg->message);
    }

    free_headers(msg->headers);
}
