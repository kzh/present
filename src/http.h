#pragma once

#ifndef HTTPUTILS
#define HTTPUTILS

struct header {
    struct header *next;
    char *name;
    char *value;
};

struct http_request {
    struct header *headers; 
    int headers_count;
    char *raw, *method, *path, *version;
};

void parse_http_req(struct http_request *req, char *buffer);
char* get_header_from(struct http_request *req, char *search); 
void free_http_req(struct http_request *req);
#endif
