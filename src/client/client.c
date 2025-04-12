#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"

int main() {
    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[1024] = {0};
    int bytes_received;

    // create a socket using IPv4 and TCP
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation error");
        exit(EXIT_FAILURE);
    }

    // set up the server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // convert IPv4 address from text to binary form
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    // connect to the server
    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connection failed");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
    printf("Connected to server %s:%d\n", SERVER_IP, PORT);

    // receive data from the server
    bytes_received = read(sock_fd, buffer, sizeof(buffer) - 1);
    if (bytes_received < 0) {
        perror("read error");
    } else {
        buffer[bytes_received] = '\0';
        printf("message from server: %s\n", buffer);
        printf("the size of buffer is: %zu bytes\n", sizeof(buffer));
    }

    close(sock_fd);
    return 0;
}