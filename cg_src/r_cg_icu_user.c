#include "r_cg_macrodriver.h"
#include "r_cg_icu.h"
#include "r_cg_userdefine.h"

extern unsigned int ContadorPulsosTac;

#pragma vector = VECT_ICU_IRQ5
#if FAST_INTERRUPT_VECTOR == VECT_ICU_IRQ5
__fast_interrupt static void r_icu_irq5_interrupt(void)
#else
__interrupt static void r_icu_irq5_interrupt(void)
#endif
{
	ContadorPulsosTac++;
}

