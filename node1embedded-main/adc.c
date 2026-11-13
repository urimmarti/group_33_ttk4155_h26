
#include "adc.h"
#include <avr/io.h>
#include <util/delay.h>

void adc_init(void) {
	// Configure clock on PD5 (~2.45 MHz)
	DDRD |= (1 << PD5);
	TCCR1A = (1 << COM1A0);
	TCCR1B = (1 << WGM12) | (1 << CS10);
	OCR1A = 0;
}

uint8_t adc_read(uint8_t channel) {
	volatile uint8_t *adc_addr = (uint8_t *) 0x1000;
	
	// Start conversion on the requested channel
	*adc_addr = channel;
	
	// Wait for the conversion to complete (~15-50 us)
	_delay_us(50);
	
	// Read and return the result from the data bus
	return *adc_addr;
}
