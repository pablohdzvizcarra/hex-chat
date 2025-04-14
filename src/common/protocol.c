#include "protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <time.h>
#include <ctype.h>

// ------------------------------------------------------------------------------------
// Helper function to convert 64-bit integers to/from network byte order.
// Since htonll() is not standard, we implement our own.

// Convert 64-bit (8 bytes) integer to a network order.
// uint64_t htonll(uint64_t value) {
//     // uint32_t high = htonl((uint32_t)(value >> 32));
//     // uint32_t low = htonl((uint32_t)(value & 0xFFFFFFFFULL));
//     // return (((uint64_t) low) << 32) | high;
// }

// ------------------------------------------------------------------------------------
//
// the binary message format is as follows:
// 1 byte   : msg_type
// 2 byte   : flags
// 8 bytes  : timestamp (current time, in network byte order)
// 1 byte   : payload length (assumed < 256)
// N bytes  : payload (raw ASCII)
// ------------------------------------------------------------------------------------
int encode_message(uint8_t msg_type, uint8_t flags, const char *payload,
                   uint8_t **out_buf, size_t *out_len) {
    printf("Encoding the message to be send to the server");
    // get payload length. (Assuming it fits within 1 byte)
    uint8_t payload_len = (uint8_t) strlen(payload);

    // total message size: 1 + 1 + 8 + 1 + payload_len.
    *out_len  = 1 + 1 + 8 + 1 + payload_len;
    *out_buf = (uint8_t *) malloc(*out_len);
    if (!(*out_buf)) {
        return -1;
    }

    size_t offset = 0;
    // write message type.
    (*out_buf)[offset++] = msg_type;
    // write flags.
    (*out_buf)[offset++] = flags;

    // write timestamp
    uint64_t timestamp = (uint64_t) time(NULL);
    uint64_t net_timestamp = htonll(timestamp);
    memcpy(*out_buf + offset, &net_timestamp, sizeof(net_timestamp));
    offset += sizeof(net_timestamp);

    // write payload length.
    (*out_buf)[offset++] = payload_len;

    // write payload
    memcpy(*out_buf + offset, payload, payload_len);
    offset += payload_len;

    // (optional: check that offset == *out_len)
    printf("Finish to encode the message to be send to the server");
    return 0;
}

// ------------------------------------------------------------------------------------
// decode_message: parses a binary message into a Message struct.
//
// The input buffer must be at least 11 bytes (1+1+8+1) and contain the declared payload
// ------------------------------------------------------------------------------------
int decode_message(const uint8_t *buf, size_t buf_len, Message *msg) {
    if (!buf || !msg) {
        return -1;
    }

    // Minimum size when payload length is 0 is 11 bytes.
    if (buf_len < 11) {
        return -1;
    }

    size_t offset = 0;
    msg->msg_type = buf[offset++];
    msg->flags = buf[offset++];

    // read the 8-byte timestamp
    uint64_t net_timestamp;
    memcpy(&net_timestamp, buf + offset, sizeof(net_timestamp));
    offset += sizeof(net_timestamp);
    msg->timestamp = ntohll(net_timestamp);

    // read payload length.
    msg->payload_len = buf[offset++];

    // check if the buffer contains enough bytes.
    if (buf_len < offset + msg->payload_len) {
        return -1;
    }

    // allocate memory for payload (+1 for null termination)
    msg->payload = (char *) malloc(msg->payload_len + 1);
    if (!msg->payload) {
        return -1;
    }

    memcpy(msg->payload, buf + offset, msg->payload_len);
    msg->payload[msg->payload_len] = '\0';

    return 0;
}

// ----------
// to_hex_string: converts raw bytes to a human-readable hex string.
//
// Each byte is represented as 2 hex digits. the returned string is dynamically allocated.
// ----------
char *to_hex_string(const uint8_t *buf, size_t len) {
    if (!buf) {
        return NULL;
    }

    // allocate (len*2 + 1) bytes for hex representation.
    char *hex_str = (char *) malloc(len * 2 + 1);
    if (!hex_str) {
        return NULL;
    }

    for (size_t i = 0; i < len; i++) {
        sprintf(hex_str + i * 2, "%02x", buf[i]);
    }

    hex_str[len * 2] = '\0';
    return hex_str;
}

// ------------------------------------------------------------------------------------
// from_hex_string: converts a hex string back into raw bytes.
// The hex string length must be even.
// ------------------------------------------------------------------------------------
uint8_t *from_hex_string(const char *hex_str, size_t *out_len) {
    if (!hex_str || !out_len) {
        return NULL;
    }

    size_t hex_len = strlen(hex_str);
    if (hex_len % 2 != 0) {
        return NULL;
    }

    *out_len = hex_len / 2;
    uint8_t *buf = (uint8_t *) malloc(*out_len);
    if (!buf) {
        return NULL;
    }

    for (size_t i = 0; i < *out_len; i++) {
        unsigned int byte;
        if (sscanf(hex_str + i * 2, "%2x", &byte) != 1) {
            free(buf);
            return NULL;
        }

        buf[i] = (uint8_t) byte;
    }

    return buf;
}