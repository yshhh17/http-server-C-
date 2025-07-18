#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>

int main() {
	int client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(client_fd < 0) {
		fprintf(stderr, "error creating socket %s\n", strerror(errno));
		return 1;
	}
	struct sockaddr_in server_address = {
		.sin_family = AF_INET,
		.sin_port = htons(4231),
	};

	if(inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) < 0) {
		perror("invalid address\n");
		exit(1);
	}

	if (connect(client_fd, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
		fprintf(stderr,"error connecting %s\n", strerror(errno));
		return 1;
	}

	char *msg = "hello from client\n";
	write(client_fd, msg, strlen(msg));

	char buffer[1024];
	int bytes = read(client_fd, buffer, sizeof(buffer)-1);
	if(bytes > 0) {
		buffer[bytes] = '\0';
		printf("Server says: %s\n", buffer);
	}

	close(client_fd);
	return 0;
}


