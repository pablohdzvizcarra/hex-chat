#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"

// Helper function to convert a 64-bit intger to network byte order.
// uint64_t htonll_helper(uint64_t value) {
//     // separate the 64-bit integer into two 32-bit halves.
//     uint32_t high_part htonl((uint32_t)(value >> 32));
//     uint32_t low_part htonl((uint32_t)(value & 0xFFFFFFFFLL));

//     return (((uint64_t) low_part) << 32) | high_part;
// }

/**
 * send_message() - Composes and sends a message according to the custom protocol.
 * 
 * @param sockfd: The socket file descriptor (assumed to be connected).
 * @param msg_type: The message type (e.g., 0x01 for a chat message).
 * @param flags: A byte of flags (e.g., 0x00 if no flags are set).
 * @param payload: The payload string to be sent.
 * 
 * The function builds a binary message containing:
 *  1 byte  : msg_type
 *  1 byte  : flags
 *  8 byte  : timestamp (in network byte order)
 *  1 byte  : payload length (assume length < 256)
 *  N bytes : payload data
 * 
 * The complete message is then sent by send(). 
 */
void send_message(int sockfd, uint8_t msg_type, uint8_t flags, const char *payload) {
    // calculate the payload length. For simplicity, we assume it fits in 1 byte.
    uint8_t payload_len = (uint8_t) strlen(payload);

    // total message size: type (1) + flags (1) + timestamp (8) + length (1) + payload.
    size_t message_size = 1 + 1 + 8 + 1 + payload_len;

    // allocate a buffer for the message
    uint8_t *buffer = malloc(message_size);
    if (!buffer) {
        perror("malloc failed");
        return;
    }

    size_t offset = 0;

    // 1. write message type
    buffer[offset++] = msg_type; // e.g. 0x01
    
    // 2. write message flags
    buffer[offset++] = flags; // e.g. 0x00

    // 3. write timestamp
    // get the current time in seconds since epoch
    uint64_t timestamp = (uint64_t) time(NULL);
    // convert timestamp into network byte order
    timestamp = htonll(timestamp);
    //copy the 8 bytes of the timestamp into the buffer
    memcpy(buffer + offset, &timestamp, sizeof(timestamp));
    offset += sizeof(timestamp);

    // 4. write payload length
    buffer[offset++] = payload_len;

    // 5. write payload
    memcpy(buffer + offset, payload, payload_len);
    offset += payload_len;

    // 6. send the complete message.
    ssize_t sent_bytes = send(sockfd, buffer, message_size, 0);
    if (sent_bytes < 0) {
        perror("send failed");
    } else if ((size_t)sent_bytes != message_size) {
        fprintf(stderr, "Partial send. Sent only %zd of %zu bytes.\n", sent_bytes, message_size);
    } else {
        printf("Message sent successfully (total %zu bytes).\n", message_size);
    }

    free(buffer);
}

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

    uint8_t msg_type = 0x01;
    uint8_t flags = 0x00;
    const char *payload = "Hello";

    while (1) {
        int BUFFER_INPUT_SIZE = 255;
        char input[BUFFER_INPUT_SIZE];
        printf("Enter a string: ");

        if (fgets(input, BUFFER_INPUT_SIZE, stdin) != NULL) {
            // optionally remove the training newline character if it exists.
            size_t len = strlen(input);
            if (len > 0 && input[len - 1] == '\n') {
                input[len - 1] = '\0';
            }
            printf("You entered: %s\n", input);
        } else {
            printf("Error reading input.\n");
        }

        send_message(sock_fd, msg_type, flags, input);
    }


    close(sock_fd);
    return 0;
}