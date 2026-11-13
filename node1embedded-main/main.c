// AI assistance disclosure: Parts of this file were developed with assistance from ChatGPT and Gemini.


#define F_CPU 4915200UL

#include "uart.h"
#include "joystick.h"
#include "adc.h"
#include "stdout_control.h"
#include "oled.h"
#include "spi.h"
#include "io_board.h"
#include "can.h"
#include "mcp2515.h"
#include "menu.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>


void xmem_init(void) {
	// 1. Enable External Memory Interface (XMEM)
	MCUCR |= (1 << SRE);
	
	// 2. Release PC4-PC7 pins (JTAG) if necessary, or adjust XMM
	// Keep only the required address lines active on PORTC
	SFIOR |= (1 << XMM2);
	SFIOR &= ~((1 << XMM1) | (1 << XMM0));
}




// Define the global streams
FILE uart_stream = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);
FILE oled_stream = FDEV_SETUP_STREAM(oled_putchar, NULL, _FDEV_SETUP_WRITE);

// Function to change the printf destination dynamically
void stdout_redirect(stdout_target_t target) {
	if (target == STDOUT_OLED) {
		stdout = &oled_stream;
		} else {
		stdout = &uart_stream;
	}
}


int main(void) {
	// 1. Initialize general system peripherals
	xmem_init();
	UART_Init(MYUBRR);
	sei();
	spi_init();
	oled_init();
	joystick_init();

	stdout_redirect(STDOUT_UART);
	printf("=== NODE 1: INITIALIZING CAN ===\n");

	if (can_init_NORMAL() != 0) {
		printf("Critical error: CAN could not be initialized on Node 1.\n");
		while(1);
	}
	printf("--> SUCCESS! Node 1 ready and on the CAN bus.\n");

	can_message_t send_msg_test = {
		.id = 1,
		.length = 3,
		.data = {100, 200, 50}
	};
	can_message_t recv_msg;
	uint8_t counter_n1 = 0;

	
	
	adc_raw_data_t joystick_data = adc_read_all();
	can_message_t send_joystick_msg = {
		.id = 0,
		.length = 4,
		.data = {joystick_data.joy_x,joystick_data.joy_y, joystick_data.slider_x, joystick_data.slider_y}
	};
	
	uint8_t last_recorded_goals = 0;
	uint8_t current_match_goals = 0;
	
	stdout_redirect(STDOUT_OLED);
	menu_update_display();

/*
	// 3. Non-blocking main loop
	while (1) {
		// Read navigation data from the IO Board via SPI
		spi_select_slave(SPI_SLAVE_IO);
		spi_write_byte(0x04);
		spi_read_byte(); // Ignore 'right' byte
		spi_read_byte(); // Ignore 'left' byte
		uint8_t nav_val = spi_read_byte(); // 'nav' byte
		spi_deselect_slave(SPI_SLAVE_IO);
		stdout_redirect(STDOUT_UART);
		printf("nav val %d\r\n", nav_val);

		// Update FSM and OLED with the new reading
		stdout_redirect(STDOUT_OLED);
		menu_update(nav_val);

		// Periodic 500 ms sampling
		_delay_ms(300);
	}*/
	while (1) {
	
		
		joystick_data = adc_read_all();
		// 1. Send message from Node 1 to Node 2
		send_joystick_msg.data[0] = joystick_data.joy_x;
		send_joystick_msg.data[1] = joystick_data.joy_y;
		send_joystick_msg.data[2] = joystick_data.slider_x;
		send_joystick_msg.data[3] = joystick_data.slider_y;
		
		stdout_redirect(STDOUT_UART);
		can_send(&send_joystick_msg);
		printf("-> Node 1 sent CAN message\n");
		printf("X: %d\nY: %d\n", joystick_data.joy_x, joystick_data.joy_y);
		uint8_t cam_x = 0, cam_y = 0;
		if (camera_get_target(&cam_x, &cam_y)) {
			printf("-> CAM RX [X: %3d, Y: %3d]\n", cam_x, cam_y);
		}
		
		stdout_redirect(STDOUT_OLED);

		// 2. Check whether Node 2 has sent anything
		uint8_t goal_changed_flag = 0;
		if (can_receive(&recv_msg)) {
			// Assume ID 2 carries the goals in the first byte (data[0])
			if (recv_msg.id == 2) {
				uint8_t incoming_goals = recv_msg.data[0];
				
				// If the game is active and the goals have changed since the last reading:
				if (is_in_game() && (incoming_goals != last_recorded_goals)) {
					current_match_goals = incoming_goals;
					last_recorded_goals = incoming_goals;
					goal_changed_flag = 1; // Set the boolean flag to update the OLED
				}
			}
		}

		// C. Read the 'nav' state from the IO Board in this iteration
		uint8_t nav_val = menu_read_nav_raw();

		// D. If the game is being exited (when is_in_game becomes 0),
		// reset the local goal counter for the next game.
		if (!is_in_game()) {
			last_recorded_goals = 0;
			current_match_goals = 0;
		}

		// E. Call the centralized function in menu.c, passing nav and the goal state
		handle_nav_and_game(nav_val, goal_changed_flag, current_match_goals);

		// Short pause in the main loop
		_delay_ms(50);
		

	}
	
	return 0;
}