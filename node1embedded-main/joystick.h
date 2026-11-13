


#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include <stdint.h>

// Joystick directions
typedef enum {
	LEFT,
	RIGHT,
	UP,
	DOWN,
	NEUTRAL
} joystick_dir_t;

// Raw data read in an ADC burst
typedef struct {
	uint8_t joy_x;
	uint8_t joy_y;
	uint8_t slider_x;
	uint8_t slider_y;
} adc_raw_data_t;

// Processed joystick state
typedef struct {
	int8_t x;           // -100% to +100%
	int8_t y;           // -100% to +100%
	joystick_dir_t dir; // LEFT, RIGHT, UP, DOWN, NEUTRAL
} joystick_position_t;

// Processed Touchpad / Slider state
typedef struct {
	uint8_t x;          // 0% to 100%
	uint8_t y;          // 0% to 100%
} slider_position_t;

// Function declarations
void joystick_init(void);

// 1. Single function to read the hardware
adc_raw_data_t adc_read_all(void);

// 2. Conversion / scaling functions
joystick_position_t joystick_process(adc_raw_data_t raw);
slider_position_t   slider_process(adc_raw_data_t raw);

#endif /* JOYSTICK_H_ */