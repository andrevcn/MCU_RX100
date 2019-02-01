/**
 * @file 		Entradas.h
 * @brief  		ARQUIVO DE MANIPULAÇÂO do ADC para leitura/controle das entradas "Digitais"
 *
 * @author 		André Machado
 * @version 	1.0
 * @date 		03/10/2018
 *
 * @par 		Historico de revisao:
 *    			V1.0 - Versao inicial.
 */

#ifndef __ENTRADAS_H
#define __ENTRADAS_H

/* Inclusoes de arquivos de cabecalho ------------------------------------------------------------*/
#include "r_cg_macrodriver.h"
#include "r_cg_s12adb.h"


/* Prototipo de funções-------------- ------------------------------------------------------------*/
void ADC_MainTask(void);
uint16_t ADC_GetValue(ad_channel_t channel);
uint16_t average_calc( uint16_t *vector_ptr, uint8_t vector_size );


#endif /*__ENTRADAS_H */


/*****END OF FILE****/

