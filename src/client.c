#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "base64.h"
#include "http.h"

int main(int argc, char *argv[]) {
	int client_connection;
	char *proxy_host, *proxy_user, *proxy_password;
	int proxy_port;
	char *host, *path;
	struct hostent *host_name;
	struct sockaddr_in host_address;
	int ind;

	if (argc < 2) {
		fprintf(
		    stderr,
		    "Usage: %s: [-p "
		    "http://[username:password@]proxy-host:proxy-port] <URL>\n",
		    argv[0]);
		return 1;
	}
	proxy_host = proxy_user = proxy_password = host = path = NULL;
	ind = 1;
	if (!strcmp("-p", argv[ind])) {
		if (!parse_proxy_param(argv[++ind], &proxy_host, &proxy_port,
				       &proxy_user, &proxy_password)) {
			fprintf(stderr,
				"Error - malformed proxy parameter '%s'.\n",
				argv[2]);
			return 2;
		}
		ind++;
	}

	if (parse_url(argv[ind], &host, &path) == -1) {
		fprintf(stderr, "Error - malformed URL '%s'. \n", argv[1]);
		return 1;
	}
	if (proxy_host) {
		printf("Connecting to host '%s'\n", host);
		host_name = gethostbyname(proxy_host);
	} else {
		printf("Connecting to host '%s'\n", host);
		/*host_name = gethostbyname(host);*/
		host_name = gethostent();
	}
	if (!host_name) {
		perror("Error in name resolution");
		return 3;
	}

	client_connection = socket(PF_INET, SOCK_STREAM, 0);
	if (!client_connection) {
		perror("Unable to create local socket");
		return 2;
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

	http_get(client_connection, path, host, proxy_host, proxy_user,
		 proxy_password);
	display_result(client_connection);
	printf("Shutting down.\n");

	if (close(client_connection) == -1) {
		perror("Error closing client connection");
		return 5;
	}
	return 0;
}
