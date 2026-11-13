
#ifndef CAN_H_
#define CAN_H_

#include <stdint.h>

// Standard CAN message structure (CAN 2.0A - 11-bit ID)
typedef struct {
    uint16_t id;        // Message ID (11 bits)
    uint8_t length;     // Data length (DLC: 0 to 8 bytes)
    uint8_t data[8];    // Data payload
} can_message_t;

/**
 * @brief Initializes the CAN module and configures the MCP2515 in Loopback mode.
 * @return uint8_t 0 if successful.
 */
uint8_t can_init_NORMAL(void);

/**
 * @brief Sends a CAN message using the TX0 buffer.
 * @param msg Pointer to the CAN message to send.
 */
void can_send(const can_message_t *msg);

/**
 * @brief Reads a CAN message received in RX0.
 * @param msg Pointer to the structure where the message will be stored.
 * @return uint8_t 1 if a message was received, 0 if there are no messages.
 */
uint8_t can_receive(can_message_t *msg);

#endif /* CAN_H_ */