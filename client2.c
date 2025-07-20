#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

void* msg_handler(void* arg) {
        int client_fd = *((int *) arg);
	free(arg);
        char buffer[1024];
        while (1) {
                memset(buffer, 0, sizeof(buffer));
                int bytes_read = read(client_fd, buffer, sizeof(buffer)-1);
                if(bytes_read <=0) {
                        printf("client disconnected\n");
                        break;
                }

                buffer[bytes_read] = '\0';
                printf("\n>> %s\n", buffer);
        }
	close(client_fd);
}

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

        if(connect(client_fd, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
                fprintf(stderr, "error connecting to server: %s\n", strerror(errno));
                return 3;
        }

	int *fd_p = malloc(sizeof(int));
	*fd_p = client_fd;

        pthread_t pid;
        pthread_create(&pid, NULL, msg_handler, (void *) fd_p);
        pthread_detach(pid);

        char input[1024];
	char* intial = ">> ";
	write(client_fd, intial, strlen(intial));
        while(1) {
                fgets(input, sizeof(input), stdin);
                input[strcspn(input,"\n")] = 0;

                if (strcmp(input, "exit") == 0) {
                        break;
                }

                write(client_fd, input, strlen(input));
        }
        close(client_fd);
	return 0;
}

