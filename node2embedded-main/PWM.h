
#ifndef PWM_H_
#define PWM_H_

#include <stdint.h>

void pwm_init();
void servo_set_duty_us(uint32_t duty_us);

#endif /* PWM_H_ */