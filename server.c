#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>

int client_fds[2];

void broadcast_to_all(int sender_fd, char* message) {
	for (int i=0;i<2;i++) {
		if(client_fds[i] != sender_fd) {
			write(client_fds[i], message, strlen(message));
		}
	}
}

void* client_handler(void* arg) {
	int client_fd = *((int *) arg);
	free(arg);

	char buffer[1024];

	while(1) {
		memset(buffer, 0, sizeof(buffer));
		int bytes_read = read(client, buffer, sizeof(buffer)-1);
		
		if (bytes_read <=0) {
			printf("client disconnected\n");
			close(client_fd);
			break;
		}

		buffer[bytes_read] = '\0';

		if (strncmp(buffer, "exit", 4) == 0) {
			printf("client requested exit\n");
			close(client_fd);
			break;
		}

		broadcast_to_all(client_fd, buffer);

	}

int main() {
	int server_fd;
	server_fd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server_address = {
		.sin_family = AF_INET,
		.sin_port = htons(4231),
		.sin_addr = { htonl(INADDR_ANY) },
	};

	if (bind(server_fd, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
			fprintf(stderr, "error binding the socket %s\n", strerror(errno));
			return 1;
	}

	int backlogs = 5;
	if (listen(server_fd, backlogs) == -1) {
		fprintf(stderr, "error while enabling listen %s\n", strerror(errno));
		return 1;
	}

	struct sockaddr_in client_address[2];
	int client_add_len = sizeof(client_address);

	for (int i=0;i<2;i++) {
		client_fds[i] = accept(server_fd, (struct sockaddr *) &client_address[i], &client_add_len);
		if (client_fds[i] < 0) {
			fprintf(stderr, "error in accepting %s\n", strerror(errno));
		}
		int *fd_ptr = malloc(sizeof(int));
		*fd_ptr = client_fds[i];

		pthread_t id;

		pthread_create(&id, NULL, client_handler, (void *) fd_ptr);
		pthread_detach(id);
	}


	//struct sockaddr_in client_address;
	//int client_add_len = sizeof(client_address);

	//client_fd = accept(server_fd, (struct sockaddr *) &client_address, &client_add_len);
	//if (client_fd < 0) {
	//	fprintf(stderr, "error in accept %s\n", strerror(errno));
	//	return 1;
	//}

	//printf("client connected!!\n");

	//char buffer[1024];
	//int bytes = read(client_fd, buffer, sizeof(buffer)-1);
	//if (bytes > 0) {
	//	buffer[bytes] = '\0';
	//	printf("server says: %s\n", buffer);
	//	char *response = "hello client\n";
	//	write(client_fd, response, strlen(response));
	//}

	//close (client_fd);
	close(server_fd);
	return 0;
}
