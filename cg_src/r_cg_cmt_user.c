/***********************************************************************************************************************
* File Name    : r_cg_cmt_user.c
* Version      : Applilet4 for RX111 V1.00.00.05 [06 Jun 2013]
* Device(s)    : R5F51115AxFL
* Tool-Chain   : IAR Systems icc78k0r
* Description  : This file implements device driver for CMT module.
* Creation Date: 6/9/2014
***********************************************************************************************************************/


/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_cmt.h"
#include "r_cg_userdefine.h"
#include "r_user_hwrctrl.h"
#include "Buzzer.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
uint8_t        Sys_LedMatrix[LED_MATRIZ_MAX_COLUMNS+16];        /* Matriz Virtual de 8 linhas de 32 colunas cada */
uint8_t        Sys_LedMatrixBuffer[LED_MATRIZ_MAX_COLUMNS];       /* Matriz Virtual de 8 linhas de 32 colunas cada para servir de buffer*/
extern uint8_t Sys_LedMatrixBufferVirada[LED_MATRIZ_MAX_COLUMNS];       /* Matriz Virtual de 8 linhas de 32 colunas cada para servir de buffer*/
uint8_t        Sys_MatrixTempEffect2[LED_MATRIZ_MAX_COLUMNS];
uint8_t        efeito_matricial;
const     uint8_t         Sys_PB_RowVector[8] = {0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x02};
const     uint8_t         Sys_PE_RowVector[8] = {0x00,0x00,0x00,0x00,0x00,0x80,0x10,0x00};
const     uint8_t         Sys_PC_RowVector[8] = {0x80,0x40,0x20,0x10,0x00,0x00,0x00,0x00};
uint32_t        Sys_DisplayFlags =4; /* Flags de controle do display */
uint32_t        Sys_ActualDisplayMessage; /* Carrega o valor da mensagem atual sendo mostrada (0 -> nenhuma. 1-8 -> mensagens)*/
uint32_t        Sys_MaxRefreshCount = 8;
uint32_t        Sys_Refreshlatch = 0;
uint32_t        Sys_RefreshCount = 0;
memoria msg;
extern uint16_t        Sys_TickTimer;

/***********************************************************************************************************************
* Function Name: r_cmt_cmi0_interrupt
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
#pragma vector = VECT_CMT0_CMI0
#if FAST_INTERRUPT_VECTOR == VECT_CMT0_CMI0
__fast_interrupt static void r_cmt_cmi0_interrupt(void)
#else
__interrupt static void r_cmt_cmi0_interrupt(void)
#endif
{
  	Sys_TickTimer++;

	BUZ_ISR_Task();
}

/***********************************************************************************************************************
* Function Name: r_cmt_cmi1_interrupt
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
uint8_t Sys_PWMlatchEn;
#pragma vector = VECT_CMT1_CMI1
#if FAST_INTERRUPT_VECTOR == VECT_CMT1_CMI1
__fast_interrupt static void r_cmt_cmi1_interrupt(void)
#else
__interrupt static void r_cmt_cmi1_interrupt(void)
#endif
{

}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
