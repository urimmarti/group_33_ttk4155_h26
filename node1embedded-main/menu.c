// AI assistance disclosure: Parts of this file were developed with assistance from ChatGPT and Gemini.
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "oled.h"
#include "spi.h"
#include "menu.h"

static const char* main_menu[] = {
	"Play",
	"Best Score"
};
#define NUM_MENU_ITEMS 2

static uint8_t current_selection = 0;
static int8_t in_game_mode = 0;
static int8_t best_score_val = -1;

// Function to read nav and buttons via SPI (command 0x04)[cite: 2]
uint8_t menu_read_nav_raw(void) {
	spi_select_slave(SPI_SLAVE_IO);
	spi_write_byte(0x04); // Button/nav read command[cite: 2]
	uint8_t right_val = spi_read_byte(); // Byte right[cite: 2]
	spi_read_byte();                      // Byte left[cite: 2]
	uint8_t nav_val = spi_read_byte();    // Byte nav[cite: 2]
	spi_deselect_slave(SPI_SLAVE_IO);
	return nav_val;
}

// Draw the main menu after clearing the screen to avoid leftover content
void menu_update_display(void) {
	// If an oled_clear() function is available, use it; otherwise, clear the screen safely line by line:
	for (uint8_t i = 0; i < 8; i++) {
		oled_clear_line(i);
	}

	oled_pos(0, 0);
	printf("=== MAIN MENU ===");

	for (uint8_t i = 0; i < NUM_MENU_ITEMS; i++) {
		oled_pos(2 + i, 0);
		if (i == current_selection) {
			printf("> %s", main_menu[i]);
			} else {
			printf("  %s", main_menu[i]);
		}
	}
}

// Central function to manage navigation and game state on each iteration
void handle_nav_and_game(uint8_t nav_val, uint8_t goal_changed, uint8_t current_goals) {
	
	// CASE 1: MAIN MENU
	if (!in_game_mode) {
		uint8_t up = (nav_val & NAV_UP);
		uint8_t down = (nav_val & NAV_DOWN);
		uint8_t btn = (nav_val & NAV_BTN);

		// DOWN and UP movement with robust individual control
		if (down) {
			current_selection = (current_selection + 1) % NUM_MENU_ITEMS;
			menu_update_display();
			_delay_ms(150); // Short pause to allow the user to release the joystick
		}
		else if (up) {
			current_selection = (current_selection == 0) ? (NUM_MENU_ITEMS - 1) : (current_selection - 1);
			menu_update_display();
			_delay_ms(150);
		}

		// If the center button (Btn) is pressed
		if (btn) {
			_delay_ms(50); // Debounce
			
			// Clear the entire screen before entering the new sub-screen
			for (uint8_t i = 0; i < 8; i++) { oled_clear_line(i); }

			if (current_selection == 0) {
				// Option 0: Play
				in_game_mode = 1;
				
				oled_pos(0, 0);
				printf("=== PLAYER MODE ===");
				oled_pos(2, 0);
				printf("Goals: 0");
				oled_pos(4, 0);
				printf("Press SR6 to exit");
			}
			else if (current_selection == 1) {
				// Option 1: Best Score
				oled_pos(0, 0);
				if (best_score_val == -1) {
					printf("Best Score: N/A");
					} else {
					printf("Best Score: %d", best_score_val);
				}
				
				// Exact 2-second delay
				_delay_ms(2000);
				
				// After leaving Best Score, redraw the clean main menu
				menu_update_display();
			}
		}
	}
	// CASE 2: GAME SCREEN
	else {
		// During the game, check whether SR6 is pressed to exit
		spi_select_slave(SPI_SLAVE_IO);
		spi_write_byte(0x04);
		uint8_t right_val = spi_read_byte(); // Read the 'right' byte[cite: 2]
		spi_read_byte();
		spi_read_byte();
		spi_deselect_slave(SPI_SLAVE_IO);

		if (right_val & BTN_R6) {
			// Store the record (fewer goals conceded is better)
			if (best_score_val == -1 || current_goals < best_score_val) {
				best_score_val = current_goals;
			}
			
			in_game_mode = 0;       // Return to the main menu
			current_selection = 0;  // Reset selection
			
			// Clear and redraw the clean main menu
			menu_update_display();
			_delay_ms(300);         // Exit debounce
		}
		
		// If there is a new goal, update the goals line
		if (goal_changed) {
			oled_clear_line(2);
			oled_pos(2, 0);
			printf("Goals: %d", current_goals);
		}
	}
}

int8_t is_in_game(void) { return in_game_mode; }
int8_t get_best_score(void) { return best_score_val; }