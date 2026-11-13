#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>



// Initializes the TC2 block in quadrature decoder mode
void encoder_init(void);

// Reads the current accumulated encoder pulse count (relative position)
int32_t encoder_read(void);

#endif