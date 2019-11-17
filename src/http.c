#include "http.h"

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

int http_get(int connection, const char *path, const char *host,
	     const char *proxy_host, const char *proxy_user,
	     const char *proxy_password) {
	static char get_command[MAX_GET_COMMAND];
	if (proxy_host) {
		sprintf(get_command, "GET http://%s/%s HTTP/1.1\r\n", host,
			path);
	} else {
		sprintf(get_command, "GET /%s HTTP/1.1\r\n", path);
	}
	if (send(connection, get_command, strlen(get_command), 0) == -1) {
		return -1;
	}
	sprintf(get_command, "Host: %s\r\n", host);
	if (send(connection, get_command, strlen(get_command), 0) == -1) {
		return -1;
	}
	if (proxy_user) {
		int credentials_len =
		    strlen(proxy_user) + strlen(proxy_password) + 1;
		char *proxy_credentials = malloc(credentials_len);
		char *auth_string = malloc(((credentials_len * 4) / 3) + 1);
		sprintf(proxy_credentials, "%s:%s", proxy_user, proxy_password);
		base64_encode(proxy_credentials, credentials_len, auth_string);
		sprintf(get_command, "Proxy-Authorization: BASIC %s\r\n",
			auth_string);
		if (send(connection, get_command, strlen(get_command), 0) ==
		    -1) {
			free(proxy_credentials);
			free(auth_string);
			return -1;
		}
		free(proxy_credentials);
		free(auth_string);
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

int parse_proxy_param(char *proxy_spec, char **proxy_host, int *proxy_port,
		      char **proxy_user, char **proxy_password) {
	char *login_sep, *colon_sep, *trailer_sep;
	if (!strncmp("http://", proxy_spec, 7)) {
		proxy_spec += 7;
	}
	login_sep = strchr(proxy_spec, '@');
	if (login_sep) {
		colon_sep = strchr(proxy_spec, ':');
		if (!colon_sep || (colon_sep > login_sep)) {
			fprintf(stderr, "Expected password in '%s'\n",
				proxy_spec);
			return 0;
		}
		*colon_sep = '\0';
		*proxy_user = proxy_spec;
		*login_sep = '\0';
		*proxy_password = colon_sep + 1;
		proxy_spec = login_sep + 1;
	}
	trailer_sep = strchr(proxy_spec, '/');
	if (trailer_sep) {
		*trailer_sep = '\0';
	}
	colon_sep = strchr(proxy_spec, ':');
	if (colon_sep) {
		*colon_sep = '\0';
		*proxy_host = proxy_spec;
		*proxy_port = atoi(colon_sep + 1);
		if (*proxy_port == 0) {
			return 0;
		}
	} else {
		*proxy_port = HTTP_PORT;
		*proxy_host = proxy_spec;
	}
	return 1;
}

char *read_line(int connection) {
	static int line_len = DEFAULT_LINE_LEN;
	static char *line = NULL;
	int size;
	char c;
	int pos = 0;

	if (!line) {
		line = malloc(line_len);
	}
	while ((size = recv(connection, &c, 1, 0)) > 0) {
		if ((c == '\n') && (line[pos - 1] == '\r')) {
			line[pos - 1] = '\0';
			break;
		}
		line[pos++] = c;
		if (pos > line_len) {
			line_len *= 2;
			line = realloc(line, line_len);
		}
	}
	return line;
}
