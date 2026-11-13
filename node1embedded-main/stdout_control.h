
#ifndef STDOUT_CONTROL_H_
#define STDOUT_CONTROL_H_

#include <stdio.h>

typedef enum {
	STDOUT_UART,
	STDOUT_OLED
} stdout_target_t;

// Declaration of global stream variables and selection function
extern FILE uart_stream;
extern FILE oled_stream;

void stdout_redirect(stdout_target_t target);

#endif