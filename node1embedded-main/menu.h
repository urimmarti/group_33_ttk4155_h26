#ifndef MENU_H
#define MENU_H

#include <stdint.h>

// Bit definitions for NAV (according to the command 0x04 table)[cite: 2]
#define NAV_UP    (1 << 4)
#define NAV_DOWN  (1 << 3)
#define NAV_BTN   (1 << 0)

// Bits for the RIGHT byte (to detect SR6/R6)[cite: 2]
#define BTN_R6    (1 << 5)

void menu_init(void);
uint8_t menu_read_nav_raw(void);
void menu_update_display(void);

// Centralized function that handles screen logic according to the input
void handle_nav_and_game(uint8_t nav_val, uint8_t goal_changed, uint8_t current_goals);

// State and best-score control
int8_t is_in_game(void);
int8_t get_best_score(void);

#endif