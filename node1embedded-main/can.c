
#include "can.h"
#include "mcp2515.h"
#include <stdio.h>

uint8_t can_init_NORMAL(void) {
	if (mcp2515_init() != 0) {
		printf("mcp init failed\n");
		return 1;
	}

	// ==========================================
	// CNFx REGISTER CONFIGURATION (Baud rate)
	// ==========================================
	// These values configure the oscillator and timing segments (Propagation, Phase1, Phase2, SJW).
	// They must exactly match the Node 2 parameters for communication to be valid.
	// (Common example for a 16 MHz crystal at 125 kbps / 250 kbps in the NTNU lab):
	
	mcp2515_write(MCP_CNF1, 0x03); // SJW = 1 TQ, BRP = 3 (divides the clock)
	mcp2515_write(MCP_CNF2, 0xB5); // BTLMODE = 1, Sample configurable, Phase1 and PropSeg
	mcp2515_write(MCP_CNF3, 0x01); // Phase2 and segment length
	
	mcp2515_bit_modify(MCP_RXB0CTRL, 0x60, 0x60);
	mcp2515_bit_modify(MCP_RXB1CTRL, 0x60, 0x60);

	mcp2515_bit_modify(MCP_CANCTRL, MODE_MASK, MODE_NORMAL);

	// Verify that the mode changed correctly
	uint8_t mode = mcp2515_read(MCP_CANSTAT);
	if ((mode & MODE_MASK) != MODE_NORMAL) {
		printf("Error entering NORMAL mode! CANSTAT: 0x%02X\n", mode);
		return 1;
	}

	printf("CAN initialized in NORMAL MODE.\n");
	return 0;
}

void can_send(const can_message_t *msg) {
	if (msg == NULL) return;

	// 1. Write standard ID (11 bits) to TXB0SIDH and TXB0SIDL
	// SIDH = ID[10:3], SIDL = ID[2:0] shifted 5 bits to the left
	mcp2515_write(MCP_TXB0SIDH, (uint8_t)(msg->id >> 3));
	mcp2515_write(MCP_TXB0SIDL, (uint8_t)((msg->id & 0x07) << 5));

	// 2. Write data length DLC (0-8)
	mcp2515_write(MCP_TXB0DLC, msg->length & 0x0F);

	// 3. Load data into the TXB0D0..D7 buffer
	for (uint8_t i = 0; i < msg->length; i++) {
		mcp2515_write(MCP_TXB0D0 + i, msg->data[i]);
	}

	// 4. Request transmission (RTS for TX0 -> buffer mask 1)
	mcp2515_request_to_send(0x01);
}

uint8_t can_receive(can_message_t *msg) {
	if (msg == NULL) return 0;

	// Check whether there is a message in RX0 (RX0IF bit in CANINTF)
	uint8_t interrupt_flags = mcp2515_read(MCP_CANINTF);

	if (interrupt_flags & MCP_RX0IF) {
		// 1. Read ID
		uint8_t sidh = mcp2515_read(MCP_RXB0SIDH);
		uint8_t sidl = mcp2515_read(MCP_RXB0SIDL);
		msg->id = (sidh << 3) | (sidl >> 5);

		// 2. Read DLC
		msg->length = mcp2515_read(MCP_RXB0DLC) & 0x0F;
		if (msg->length > 8) msg->length = 8;

		// 3. Read data
		for (uint8_t i = 0; i < msg->length; i++) {
			msg->data[i] = mcp2515_read(MCP_RXB0D0 + i);
		}

		// 4. Clear RX0 interrupt flag
		mcp2515_bit_modify(MCP_CANINTF, MCP_RX0IF, 0x00);

		return 1; // Message received
	}

	return 0; // No message available
}