// AI assistance disclosure: Parts of this file were developed with assistance from ChatGPT and Gemini.
#include "sam.h"
#include "motor.h"

// ---- PINS: VERIFY AGAINST THE "TTK4155 Motor Shield document" / shield schematic ----
// Example values. PB12 is PWMH0 (Peripheral B) according to the SAM3X datasheet,
// but whether the shield uses that pin must be confirmed in its documentation.
#define MOTOR_PWM_CH      0
#define MOTOR_PWM_PIO     PIOB
#define MOTOR_PWM_BIT     (1u << 12)   // PB12 = PWMH0, Peripheral B
#define MOTOR_DIR_PIO     PIOC
#define MOTOR_DIR_BIT     (1u << 23)   // EXAMPLE: DIRECTION pin (NOR input)
#define MOTOR_DIR_INVERT  0            // set to 1 if the direction is inverted

// PWM signal polarity after the NOR. 0 = PWM high => motor energized (the expected
// behavior). 1 = PWM low => motor energized. UNKNOWN: it depends on the NOR wiring.
// Check this BEFORE applying 12 V: with duty 0, DRV8871 inputs IN1 and IN2 must
// BOTH be low (motor stopped). If the motor is energized at duty 0, set this to 1.
#define MOTOR_PWM_ACTIVE_LOW 0

// The DRV8871 requires >= 800 ns pulse width to detect the input (datasheet 6.3, note 1).
// At 20 kHz this is ~1.6%; below 2% it is treated as 0 to avoid "phantom" duty.
#define MOTOR_MIN_PERCENT 2

#define MOTOR_PWM_PERIOD  4200u        // 84 MHz / 4200 = 20 kHz (inaudible)
#define MOTOR_MAX_PERCENT 80           // safety limit

void motor_init(void){
	PMC->PMC_PCER0 |= (1u << ID_PIOB) | (1u << ID_PIOC);
	PMC->PMC_PCER1 |= (1u << (ID_PWM - 32));

	// PWM pin -> Peripheral B
	MOTOR_PWM_PIO->PIO_ABSR |= MOTOR_PWM_BIT;
	MOTOR_PWM_PIO->PIO_PDR   = MOTOR_PWM_BIT;

	// Direction pin -> GPIO output
	MOTOR_DIR_PIO->PIO_PER  = MOTOR_DIR_BIT;
	MOTOR_DIR_PIO->PIO_OER  = MOTOR_DIR_BIT;
	MOTOR_DIR_PIO->PIO_CODR = MOTOR_DIR_BIT;

	// Dedicated channel, clock = direct MCK (does not affect CLKA, which is used by the servo).
	// Active high: CPOL = 1 -> CDTY is the high time; CDTY = 0 leaves the output low (stopped).
	// Active low: CPOL = 0 -> CDTY is the low time; CDTY = 0 leaves the output high (stopped).
	PWM->PWM_DIS = (1u << MOTOR_PWM_CH);
	PWM->PWM_CH_NUM[MOTOR_PWM_CH].PWM_CMR  = PWM_CMR_CPRE_MCK | (MOTOR_PWM_ACTIVE_LOW ? 0u : PWM_CMR_CPOL);
	PWM->PWM_CH_NUM[MOTOR_PWM_CH].PWM_CPRD = MOTOR_PWM_PERIOD;
	PWM->PWM_CH_NUM[MOTOR_PWM_CH].PWM_CDTY = 0;
	PWM->PWM_ENA = (1u << MOTOR_PWM_CH);
}

#define PWM_CLOCK 1000000UL   // 1 MHz internal PWM clock

void pwm_init_pb12(uint32_t frequency, uint8_t duty_percent)
{
    /* 1. Enable peripheral clock for PWM controller (ID 36) */
    PMC->PMC_PCER1 |= PMC_PCER1_PID36;

    /* 2. PB12 must be controlled by Peripheral B = PWMH0 */
    PIOB->PIO_ABSR |= PIO_ABSR_P12;   // Peripheral B
    PIOB->PIO_PDR  |= PIO_PDR_P12;    // Disable GPIO control

    /* Stop channel 0 while configuring */
    PWM->PWM_DIS = PWM_DIS_CHID0;

    /*
     * 3. Create CLKA = 84 MHz / 84 = 1 MHz
     *
     * PREA = 0 -> no 2^n prescaling
     * DIVA = 84
     */
    PWM->PWM_CLK =
        PWM_CLK_PREA(0) |
        PWM_CLK_DIVA(84);

    /*
     * 4. Channel 0:
     *    - use CLKA
     *    - left-aligned PWM
     *    - CPOL = 1 -> output starts HIGH
     */
    PWM->PWM_CH_NUM[0].PWM_CMR =
        PWM_CMR_CPRE_CLKA |
        PWM_CMR_CPOL;

    /* Number of PWM clock ticks per period */
    uint32_t period = PWM_CLOCK / frequency;

    /* Protect against invalid values */
    if (period < 1)
        period = 1;

    if (period > 65535)
        period = 65535;

    if (duty_percent > 100)
        duty_percent = 100;

    PWM->PWM_CH_NUM[0].PWM_CPRD = period;

    /* High-time */
    PWM->PWM_CH_NUM[0].PWM_CDTY =
        (period * duty_percent) / 100;

    /* 5. Enable PWM channel 0 */
    PWM->PWM_ENA = PWM_ENA_CHID0;
}


void motor_set(int percent){
	int forward = (percent >= 0);
	if(percent < 0) percent = -percent;
	if(percent > MOTOR_MAX_PERCENT) percent = MOTOR_MAX_PERCENT;
	if(percent < MOTOR_MIN_PERCENT) percent = 0;

	if(forward ^ MOTOR_DIR_INVERT){
		MOTOR_DIR_PIO->PIO_CODR = MOTOR_DIR_BIT;
		} else {
		MOTOR_DIR_PIO->PIO_SODR = MOTOR_DIR_BIT;
	}
	PWM->PWM_CH_NUM[MOTOR_PWM_CH].PWM_CDTYUPD = (MOTOR_PWM_PERIOD * (uint32_t)percent) / 100u;
}

void motor_stop(void){
	PWM->PWM_CH_NUM[MOTOR_PWM_CH].PWM_CDTYUPD = 0;
}

