#pragma once

#ifndef HTTPUTILS
#define HTTPUTILS

struct header {
    struct header *next;
    char *name;
    char *value;
};

struct http_message {
    struct header *headers;
    int headers_count;

    /*
    REQUEST:  1 - METHOD, 2 - PATH, 3 - VERSION
    RESPONSE: 1 - ERSION, 2 - STATUS CODE, 3 - REASON
    */
    char* info[3];
};

void parse_http_message(struct http_message *msg, char *buffer);
void free_http_message(struct http_message *msg);

void insert_header(struct http_message *msg, struct header *header);
char* get_header_from(struct http_message *msg, char *search); 

#define REQUEST_METHOD  0
#define REQUEST_PATH    1
#define REQUEST_VERSION 2

#define RESPONSE_VERSION     0
#define RESPONSE_STATUS_CODE 1
#define RESPONSE_REASON      2
#endif
