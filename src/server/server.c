#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define BACKLOG 5
#define BUFFER_SIZE 1024

int main() {
    // This variable holdes the server connection
    int server_fd;
    // This variable represents the open client connection in the client --> this server
    int client_fd;

    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];
    char *hello = "Hello from server";
    ssize_t bytes_read;

    // Create a socket connection with IPv4 addressing and TCP protocol
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // setup the address structure for binding
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // bind the socket to the specified port and IP address
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    // listen for incomming connections
    if (listen(server_fd, BACKLOG) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // accept an incomming connection (this call blocks until a connection is made)
    if ((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Accepted connnection from %s:%d\n",
        inet_ntoa(address.sin_addr), ntohs(address.sin_port));

    // send a greeting message to the client
    if (send(client_fd, hello, strlen(hello), 0) == -1) {
        perror("send failed");
    } else {
        printf("Greeting message sent.\n");
    }

    while (1) {
        // clear the buffer before each read.
        memset(buffer, 0, BUFFER_SIZE);
        bytes_read = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_read < 0) {
            perror("recv failed");
            break;
        } else if (bytes_read == 0) {
            printf("Client disconnected.\n");
            break;
        } else {
            buffer[bytes_read] = '\0';
            printf("Received (%zd bytes): %s\n", bytes_read, buffer);
        }
    }

    close(client_fd);
    close(server_fd);

    return 0;
}