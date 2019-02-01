#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"
#include "r_cg_cmt.h"
#include "r_cg_iwdt.h"
#include "r_cg_s12adb.h"
#include "r_cg_icu.h"
#include "r_cg_mtu2.h"
#include "r_cg_userdefine.h"


#pragma diag_suppress = Pm011
int __low_level_init(void);
#pragma diag_default = Pm011
void R_Systeminit(void);


void R_Systeminit(void)
{
    /* Enable writing to registers related to operating modes, LPC, CGC and software reset */
    SYSTEM.PRCR.WORD = 0xA507U;

    /* Enable writing to MPC pin function control registers */
    MPC.PWPR.BIT.B0WI = 0U;
    MPC.PWPR.BIT.PFSWE = 1U;

    /* Set peripheral settings */
    R_CGC_Create();
    R_PORT_Create();
    R_CMT0_Create();	//TImer 1ms
	R_MTU2_U0_Create();  //PWM
    R_S12AD_Create();

    /* Disable writing to MPC pin function control registers */
    MPC.PWPR.BIT.PFSWE = 0U;
    MPC.PWPR.BIT.B0WI = 1U;

    /* Enable protection */
    SYSTEM.PRCR.WORD = 0xA500U;
}

#pragma diag_suppress = Pm011
int __low_level_init(void)
#pragma diag_default = Pm011
{
    DI();
    R_Systeminit();

    return (int)(1U);
}

