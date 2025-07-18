#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>

int main() {
	int server_fd, client_fd;
	server_fd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server_address = {
		.sin_family = AF_INET,
		.sin_port = htons(4231),
		.sin_addr = { htonl(INADDR_ANY) },
	};

	if (bind(server_fd, (struct sockaddr *) &server_address, sizeof(server_address) == -1)) {
			fprintf(stderr, "error binding the socket %s\n", strerror(errno));
			return 1;
	}

	int backlogs = 5;
	if (listen(server_fd, backlogs) == -1) {
		fprint(stderr, "error while enabling listen %s\n", strerror(errno));
		return 1;
	}

	struct sockaddr_in client_address;
	int client_add_len = sizeof(client_address);

	int client_fd = accept(server_fd, (struct sockaddr *) &server_address, sizeof(server_address));
	if (client_fd < 0) {
		fprintf(stderr, "error in accept %s\n", strerror(errno));
		return 1;
	}
