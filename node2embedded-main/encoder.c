#include "sam.h"
#include "encoder.h"

#define ENC_PHA_BIT (1u << 25)   // PC25 = TIOA6 (Phase A)
#define ENC_PHB_BIT (1u << 26)   // PC26 = TIOB6 (Phase B)[cite: 4]

void encoder_init(void) {
	// 1. Enable the clocks for PIOC and the TC2 block (Channel 6 / TC6)[cite: 4]
	PMC->PMC_PCER0 |= (1u << ID_PIOC);
	PMC->PMC_PCER1 |= (1u << (ID_TC6 - 32));

	// 2. Configure BOTH pins (PC25 and PC26) to use Peripheral B function[cite: 4]
	// (This physically connects encoder signals A and B to the timer hardware block)[cite: 4]
	PIOC->PIO_ABSR |= ENC_PHA_BIT | ENC_PHB_BIT;
	PIOC->PIO_PDR   = ENC_PHA_BIT | ENC_PHB_BIT; // Disable standard PIO control and hand it over to the Peripheral[cite: 4]

	// 3. Configure the TC2 Block Mode Register (BMR) to enable full quadrature mode[cite: 4]
	// QDEN: Enables QDEC[cite: 4]
	// POSEN: Enables position mode on channel 0[cite: 4]
	// EDGPHA: Counts edges on both phases (4x resolution)[cite: 4]
	// MAXFILT: Filter to reject short noise pulses[cite: 4]
	TC2->TC_BMR = TC_BMR_QDEN | TC_BMR_POSEN | TC_BMR_EDGPHA | TC_BMR_MAXFILT(1);

	// 4. Configure the channel clock and trigger the counter start[cite: 4]
	TC2->TC_CHANNEL[0].TC_CMR = TC_CMR_TCCLKS_XC0;
	TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
}

int32_t encoder_read(void) {
	// The Counter Value register (TC_CV) stores the signed accumulated position count[cite: 4]
	return (int32_t)TC2->TC_CHANNEL[0].TC_CV;
}