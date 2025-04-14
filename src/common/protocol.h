#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    // A structure to hold a decoded message.
    typedef struct {
        uint8_t msg_type;
        uint8_t flags;
        uint64_t timestamp;
        uint8_t payload_len;
        char *payload;
    } Message;

    /**
     * encode_message - Composes a binary message using the custom protocol.
     *
     * @msg_type: The message type (e.g., 0x01 for chat message).
     * @flags: An 8-bit flags value.
     * @payload: The ASCII string to include as the payload.
     * @out_buf: Pointer to the output buffer (dynamically allocated).
     * @out_len: Pointer to the output message length.
     *
     * Returns 0 on success, or a non-zero value on error.
     * The caller is responsible for freeing the buffer returned in out_buf.
     */
    int encode_message(uint8_t msg_type, uint8_t flags, const char *payload, 
                    uint8_t **out_buf, size_t *out_len);

    /**
     * decode_message - Parses a binary message into a Message struct.
     * 
     * @buf: The input buffer containing the raw binary message.
     * @buf_len: The length of the input buffer.
     * @msg: Pointer to a message struct that will be filled.
     * 
     * Returns 0 on success, or a non-zero value on error.
     * The Message::paylod member will be dynamically allocated and must be free by the caller.
     */
    int decode_message(const uint8_t *buffer, size_t buf_len, Message *msg);

    /**
     * to_hex_string - Converts an array of raw bytes into a hex string.
     * 
     * @buf: the input byte array
     * @len: the number of bytes in the array
     * 
     * Returns a dynamically allocated string containing the hex representation.
     * The caller must free the returned string. 
     */
    char *to_hex_string(const uint8_t *buf, size_t len);

    /**
     * from_hex_string - Converts a hex string back into raw bytes.
     * 
     * @hex_str: The input hex string (should have even length).
     * @out_len: Pointer where the output length is stored.
     * 
     * Returns a dynamically allocated buffer containing the raw bytes.
     * The caller must free the returned buffer.
     */
 
#ifdef __cplusplus
}
#endif

#endif // PROTOCOL_H