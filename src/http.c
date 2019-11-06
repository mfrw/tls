#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define HTTP_PORT 80

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

	return 0;
}
