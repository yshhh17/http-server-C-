#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <string.h>

typedef struct {
	int my_fd;
	int your_fd;
} client_peer_fd;

void* client_handler(void* arg) {
	client_peer_fd* pair = (client_peer_fd*) arg;
	int client_fd = pair->my_fd;
	int peer_fd = pair->your_fd;
	free(arg);

	char buffer[1024];

	while(1) {
		memset(buffer, 0, sizeof(buffer));

		int bytes_read = read(client_fd, buffer, sizeof(buffer) -1);

		if(bytes_read <= 0) {
			printf("client disconnected or error.\n");
			char* alert = "the other user has disconnected\n";
			write(peer_fd, alert, strlen(alert));
			close(client_fd);
			close(peer_fd);
			break;
		}

		buffer[bytes_read] = '\0';

		if(strncmp(buffer, "exit", 4) == 0) {
			printf("client requested exit\n");
			close(client_fd);
			close(peer_fd);
			break;
		}

		write(peer_fd, buffer, bytes_read);
	}
	return NULL;
}

int main() {
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server_address = {
		.sin_family = AF_INET,
		.sin_port = htons(4231),
		.sin_addr = { htonl(INADDR_ANY) },
	};

	int reuse = 1;
	if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
		fprintf(stderr, "SO_REUSEADDR failed: %s\n", strerror(errno));
		return 1;
	}

	if(bind(server_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
		fprintf(stderr, "binding failed: %s\n", strerror(errno));
		return 2;
	}

	int backlogs = 3;
	if(listen(server_fd, backlogs) < 0) {
		fprintf(stderr, "error while listening: %s\n", strerror(errno));
	}

	printf("server is listening for two clients\n");

	struct sockaddr_in my_address, your_address;
	socklen_t my_addr_len = sizeof(my_address);
	socklen_t your_addr_len = sizeof(your_address);

	int client_fd1 = accept(server_fd, (struct sockaddr *) &my_address, &my_addr_len);
	if (client_fd1 < 0) {
		fprintf(stderr, "error accepting: %s\n", strerror(errno));
		return 4;
	}
	printf("client 1 connected successfully\n");
	char *greeting = "you are connected waiting for the second user to connect...\n";
	write(client_fd1, greeting, strlen(greeting));


	int client_fd2 = accept(server_fd, (struct sockaddr *) &your_address, &your_addr_len);
	if (client_fd2 < 0) {
		fprintf(stderr, "error accepting client 2: %s\n", strerror(errno));
		return 5;
	}
	printf("client 2 is connected start the chat session\n");
	greeting = "you are connected starting the chat session\n";
	write(client_fd2, greeting, strlen(greeting));

	client_peer_fd* pair1 = malloc(sizeof(client_peer_fd));
	pair1->my_fd = client_fd1;
	pair1->your_fd = client_fd2;

	client_peer_fd* pair2 = malloc(sizeof(client_peer_fd));
	pair2->my_fd = client_fd2;
	pair2->your_fd = client_fd1;

	pthread_t pid1, pid2;
	pthread_create(&pid1, NULL, client_handler, (void*) pair1);
	pthread_create(&pid2, NULL, client_handler, (void*) pair2);

	pthread_join(pid1, NULL);
	pthread_join(pid2, NULL);

	printf("server shutting down\n");
	close(server_fd);
	return 0;
}

