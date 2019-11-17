#ifndef HTTP_H
#define HTTP_H

#define HTTP_PORT 8080
#define MAX_GET_COMMAND 255
#define BUFFER_SIZE 255
#define DEFAULT_LINE_LEN 255

#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int parse_url(char *, char **, char **);
int http_get(int, const char *, const char *, const char *, const char *,
	     const char *);
void display_result(int);
int parse_proxy_param(char *, char **, int *, char **, char **);
char *read_line(int);

#endif /* end of include guard: HTTP_H */
