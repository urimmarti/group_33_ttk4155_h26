/*
 * ir.c
 *
 * Created: 29.10.2026 15:29:38
 *  Author: quetzall
 */ 


#include "sam.h"
#include "ir.h"
#include "time.h"

// Adjust this threshold according to the console readings when the ball passes
#define IR_THRESHOLD 100

static uint8_t score = 0;
static bool was_blocked = false;

void ir_init(void) {
	// 1. Enable the ADC clock in the PMC (ID_ADC = 37 -> PMC_PCER1)
	PMC->PMC_PCER1 |= (1 << (ID_ADC - 32));

	// 2. Reset the ADC
	ADC->ADC_CR = ADC_CR_SWRST;

	// 3. Configure mode (software trigger, standard prescaler)
	ADC->ADC_MR = ADC_MR_PRESCAL(2) | ADC_MR_STARTUP_SUT8;

	// 4. Enable Channel 0 (ADC0 / PA2 pin on the expansion board)
	ADC->ADC_CHER = (1 << 0);
}

uint32_t ir_read_raw(void) {
	// Start conversion by software
	ADC->ADC_CR = ADC_CR_START;

	// Wait for channel 0 conversion to finish (EOC0)
	while (!(ADC->ADC_ISR & (1 << 0)));

	// Return the 12-bit result
	return ADC->ADC_CDR[0];
}

bool ir_check_goal(void) {
	uint32_t current_val = ir_read_raw();

	// Threshold detection (if the beam is interrupted and the voltage drops/rises according to the circuit)
	// Assume it falls below the threshold when the beam is blocked:
	bool currently_blocked = (current_val < IR_THRESHOLD);

	// Debounce logic (edge detection: count only on the falling edge of the light signal)
	if (currently_blocked && !was_blocked) {
		was_blocked = true;
		score++;
		return true; // Goal registered!
	}
	
	// Reset the state when the ball leaves and the light returns
	if (!currently_blocked) {
		was_blocked = false;
	}

	return false;
}

uint8_t ir_get_score(void) {
	return score;
}