#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
	int client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (client_fd<0) {
		fprintf(stderr, "errer creating client socket %s\n", strerror(errno));
		return 1;
	}

	struct sockaddr_in server_address = {
		.sin_family = AF_INET,
		.sin_port = htons(4231),
	};

	if(inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) < 0) {
		fprintf(stderr, "error setting the IP: %s\n", strerror(errno));
		return 2;
	}

	if(connect(client_fd, (struct sockaddr *) server_address, sizeof(server_address)) == -1) {
		fprintf(stderr, "error connecting to server: %s\n", strerror(errno));
		return 3;
	}

	


