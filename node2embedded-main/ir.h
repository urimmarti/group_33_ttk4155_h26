/*
 * ir.h
 *
 * Created: 29.10.2026 15:29:48
 *  Author: quetzall
 */ 


#ifndef IR_H
#define IR_H

#include <stdint.h>
#include <stdbool.h>

// Initializes the ADC and channel 0 for the IR sensor
void ir_init(void);

// Reads the raw ADC value (0 - 4095)
uint32_t ir_read_raw(void);

// Checks whether a goal has been detected (includes threshold and debounce)
bool ir_check_goal(void);

// Returns the current accumulated score
uint8_t ir_get_score(void);

#endif