#include "http.h"

#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define HTTP_PORT 80
#define MAX_GET_COMMAND 255
#define BUFFER_SIZE 255

int parse_url(char *uri, char **host, char **path) {
	char *pos;
	pos = strstr(uri, "//");
	if (!pos) {
		return -1;
	}
	*host = pos + 2;
	pos = strchr(*host, '/');
	if (!pos) {
		*path = NULL;
	} else {
		*pos = '\0';
		*path = pos + 1;
	}
	return 0;
}

int main(int argc, char *argv[]) {
	int client_connection;
	char *host, *path;
	struct hostent *host_name;
	struct sockaddr_in host_address;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s: <URL>\n", argv[0]);
		return 1;
	}
	if (parse_url(argv[1], &host, &path) == -1) {
		fprintf(stderr, "Error - malformed URL '%s'. \n", argv[1]);
		return 1;
	}

	printf("Connecting to host '%s'\n", host);

	client_connection = socket(PF_INET, SOCK_STREAM, 0);
	if (!client_connection) {
		perror("Unable to create local socket");
		return 2;
	}

	host_name = gethostbyname(host);
	if (!host_name) {
		perror("Error in name resolution");
		return 3;
	}

	host_address.sin_family = AF_INET;
	host_address.sin_port = htons(HTTP_PORT);
	memcpy(&host_address.sin_addr, host_name->h_addr_list[0],
	       sizeof(struct in_addr));

	if (connect(client_connection, (struct sockaddr *)&host_address,
		    sizeof(host_address)) == -1) {
		perror("Unable to connect to host");
		return 4;
	}
	printf("Retrieving document: '%s'\n", path);

	http_get(client_connection, path, host);
	display_result(client_connection);
	printf("Shutting down.\n");

	if (close(client_connection) == -1) {
		perror("Error closing client connection");
		return 5;
	}
	return 0;
}

int http_get(int connection, const char *path, const char *host) {
	static char get_command[MAX_GET_COMMAND];
	sprintf(get_command, "GET /%s HTTP/1.1\r\n", path);
	if (send(connection, get_command, strlen(get_command), 0) == -1) {
		return -1;
	}
	sprintf(get_command, "Host: %s\r\n", host);
	if (send(connection, get_command, strlen(get_command), 0) == -1) {
		return -1;
	}
	sprintf(get_command, "Connection: close\r\n\r\n");
	if (send(connection, get_command, strlen(get_command), 0) == -1) {
		return -1;
	}
	return 0;
}

void display_result(int connection) {
	int recieved = 0;
	static char recv_buf[BUFFER_SIZE + 1];
	while ((recieved = recv(connection, recv_buf, BUFFER_SIZE, 0)) > 0) {
		recv_buf[recieved] = '\0';
		printf("%s", recv_buf);
	}
	printf("\n");
}
