/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software.  By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2012 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_cg_userdefine.h
* Version      : Applilet4 for RX111 V1.00.00.05 [06 Jun 2013]
* Device(s)    : R5F51115AxFL
* Tool-Chain   : IAR Systems icc78k0r
* Description  : This file includes user definition.
* Creation Date: 6/9/2014
***********************************************************************************************************************/
#ifndef _USER_DEF_H
#define _USER_DEF_H

/***********************************************************************************************************************
User definitions
***********************************************************************************************************************/
#define FAST_INTERRUPT_VECTOR 0

/* Start user code for function. Do not edit comment generated here */
/*------------------------Definições Gerais -----------------------*/
#define         MSG_MAX_NUMBER          12
#define         LED_MATRIZ_MAX_COLUMNS  32
/*------------------------Definições de Pinos-----------------------*/
/* Pinos usados no controle dos STP16CPC26MTR */
#define         MCU_DI          (1<<1)  /* PA */
#define         MCU_LE          (1<<6)  /* PA */
#define         MCU_CLK         (1<<3)  /* PA */
#define         MCU_OE          (1<<0)  /* PB */
/* Pinos utilizados na leitura das duas teclas presentes na frente do produto */
#define         HOUR_UP         (1<<7)  /* P1 */
#define         HOUR_DOWN       (1<<7)  /* P2 */
/* Definição de entrada do produto */
#define         INPUT_TURN_ON   (1<<7)  /* PJ */
/* Pinos utilizados no controle das linhas de LEDs */
#define         MCU_ROW1        (1<<1)  /* PB */
#define         MCU_ROW2        (1<<4)  /* PE */
#define         MCU_ROW3        (1<<7)  /* PE */
#define         MCU_ROW4        (1<<5)  /* PB */
#define         MCU_ROW5        (1<<4)  /* PC */
#define         MCU_ROW6        (1<<5)  /* PC */
#define         MCU_ROW7        (1<<6)  /* PC */
#define         MCU_ROW8        (1<<7)  /* PC */
#define         MCU_ROW4_1      ~(MCU_ROW4 | MCU_ROW1)
#define         MCU_ROW2_3      ~(MCU_ROW2 | MCU_ROW3)
#define         MCU_ROW5_6_7_8  ~(MCU_ROW5 | MCU_ROW6 | MCU_ROW7 | MCU_ROW8)


#define         Turn_OFF_MCU_ROW1()        	PORTB.PODR.BYTE &= ~MCU_ROW1
#define         Turn_OFF_MCU_ROW2()			PORTE.PODR.BYTE &= ~MCU_ROW2
#define         Turn_OFF_MCU_ROW3()			PORTE.PODR.BYTE &= ~MCU_ROW3
#define         Turn_OFF_MCU_ROW4()			PORTB.PODR.BYTE &= ~MCU_ROW4
#define         Turn_OFF_MCU_ROW5()			PORTC.PODR.BYTE &= ~MCU_ROW5
#define         Turn_OFF_MCU_ROW6()			PORTC.PODR.BYTE &= ~MCU_ROW6
#define         Turn_OFF_MCU_ROW7()			PORTC.PODR.BYTE &= ~MCU_ROW7
#define         Turn_OFF_MCU_ROW8()			PORTC.PODR.BYTE &= ~MCU_ROW8


#define         Turn_ON_MCU_ROW1()        	PORTB.PODR.BYTE |= MCU_ROW1
#define         Turn_ON_MCU_ROW2()			PORTE.PODR.BYTE |= MCU_ROW2
#define         Turn_ON_MCU_ROW3()			PORTE.PODR.BYTE |= MCU_ROW3
#define         Turn_ON_MCU_ROW4()			PORTB.PODR.BYTE |= MCU_ROW4
#define         Turn_ON_MCU_ROW5()			PORTC.PODR.BYTE |= MCU_ROW5
#define         Turn_ON_MCU_ROW6()			PORTC.PODR.BYTE |= MCU_ROW6
#define         Turn_ON_MCU_ROW7()			PORTC.PODR.BYTE |= MCU_ROW7
#define         Turn_ON_MCU_ROW8()			PORTC.PODR.BYTE |= MCU_ROW8


/*------------------------Controle dos pinos-----------------------*/
/* Controle dos STP16CPC26MTR */
#define         Turn_On_MCU_DI()        (PORTA.PODR.BYTE |= MCU_DI);
#define         Turn_Off_MCU_DI()       (PORTA.PODR.BYTE &= ~MCU_DI);
/*=================================================================*/
#define         Turn_On_MCU_CLK()       (PORTA.PODR.BYTE |= MCU_CLK);
#define         Turn_Off_MCU_CLK()      (PORTA.PODR.BYTE &= ~MCU_CLK);
/*=================================================================*/
#define         Turn_On_MCU_LE()        (PORTA.PODR.BYTE |= MCU_LE);
#define         Turn_Off_MCU_LE()       (PORTA.PODR.BYTE &= ~MCU_LE);
/*=================================================================*/
#define         Turn_On_MCU_OE()        (PORTB.PODR.BYTE |= MCU_OE);
#define         Turn_Off_MCU_OE()       (PORTB.PODR.BYTE &= ~MCU_OE);



/*=================================================================*/
/*-------------Definição dos pinos do display ---------------------*/
#define         RS_Off()				PORT4.PODR.BYTE &= ~(1<<0)
#define         RS_On()					PORT4.PODR.BYTE |=  (1<<0)

#define         EN_Off()				PORTE.PODR.BYTE &= ~(1<<1)
#define         EN_On()					PORTE.PODR.BYTE |=  (1<<1)

#define			HD44780_D7_Off()		PORTB.PODR.BYTE &= ~(1<<0)
#define			HD44780_D7_On()			PORTB.PODR.BYTE |=  (1<<0)

#define			HD44780_D6_Off()		PORTA.PODR.BYTE &= ~(1<<6)
#define			HD44780_D6_On()			PORTA.PODR.BYTE |=  (1<<6)

#define			HD44780_D5_Off()		PORTA.PODR.BYTE &= ~(1<<3)
#define			HD44780_D5_On()			PORTA.PODR.BYTE |=  (1<<3)

#define			HD44780_D4_Off()		PORTA.PODR.BYTE &= ~(1<<1)
#define			HD44780_D4_On()			PORTA.PODR.BYTE |=  (1<<1)

/*=================================================================*/
/*-------------Definição dos pinos do Buzzer ---------------------*/
#define         Turn_Off_Buzzer()			PORTC.PODR.BYTE &= ~(1<<7)
#define         Turn_On_Buzzer()			PORTC.PODR.BYTE |=  (1<<7)

/*=================================================================*/
/*---------------------- Definição de flags -----------------------*/
/* Sys_DisplayFlags */
#define         CanUpdateBuffer                 0x00000001
#define         NeedToFlowWithNothing           0x00000002
#define         NeedToChangeActualMessage       0x00000004
#define         NeedToStopFlow                  0x00000008
#define         NeedToNormalFlow                0x00000010



/* Sys_TypeOfMsgOnDisplay */
#define         NormalMessage                   0
#define         EditMessage                     1
#define         OtherMessage                    2
#define         SetRTC                          3
#define         ModoTeste                       4
#define         SetFatorK                       5



/*----------------- Definição das informações das mensagem -------------------*/
#define         MSG_INFO_EFEITO             0//FICA
#define         MSG_INFO_HABILITADA         1//FICA
#define         MSG_INFO_VEL_DE_TROCA       2//FICA
#define         MSG_INFO_QUADROS_PARADOS    4//FICA

/*============================================================================*/
/*      FUNCTIONS DEFINITIONS                                                 */
/*============================================================================*/
/* System General Flags */
#define         ADC_RUNNING             0x0001
#define         ADC_WAITING             0x0002


/* ReadADC */
#define         ADC_READ_DONE           0x8000
#define         NUMBER_OF_ADC_CHANNELS  23
#define         AVERAGE_READ            0
#define         MOVING_AVERAGE_READ     1
#define         HIGHEST_VALUE           2
#define         LOWEST_VALUE            3
#define         CHANNEL_0               0
#define         CHANNEL_1               1
#define         CHANNEL_2               2
#define         CHANNEL_3               3
#define         CHANNEL_4               4
#define         CHANNEL_5               5
#define         CHANNEL_6               6
#define         CHANNEL_7               7
#define         CHANNEL_8               8
#define         CHANNEL_9               9
#define         CHANNEL_10              10
#define         CHANNEL_11              11
#define         CHANNEL_12              12
#define         CHANNEL_13              13
#define         CHANNEL_14              14
#define         CHANNEL_15              15
#define         CHANNEL_16              16
#define         CHANNEL_17              17
#define         CHANNEL_18              18
#define         CHANNEL_19              19
#define         CHANNEL_20              20
#define         CHANNEL_21              21
#define         CHANNEL_22              22
/* ---------------------- TIPOS DE EFEITO DE ROTAÇÃO ---------------------- */
#define         ROT_EFEITO_ESTATICA             0 //a
#define         ROT_EFEITO_DIREITA              1 //b
#define         ROT_EFEITO_ESQUERDA             2 //c
#define         ROT_EFEITO_CIMA                 3 //d
#define         ROT_EFEITO_BAIXO                4 //e
#define         ROT_EFEITO_QUICANDO             5 //f
#define         ROT_EFEITO_LASER                6 //g
#define         ROT_EFEITO_MATRICIAL            7 //h
#define         ROT_EFEITO_PACMAN               8 //i
#define         ROT_EFEITO_REVELAR              9 //j
#define         ROT_EFEITO_REVELAR_INV          10//k
#define         ROT_EFEITO_DISSOLVER            11//l
#define         ROT_EFEITO_SOBREPOR             12//m
#define         EMPURRAR_ESQUERDA               13//n
#define         EMPURRAR_BAIXO                  14//o
#define         EMPURRAR_CIMA                   15//p
#define         EFEITO_SCROLL                   16//q


/* -------------------- SENSOR DE TEMPERATURA INTERNO ----------------------- */
/*       PN             R@25ºc      Constante B@25-85ºC                       */
/* NCP18XH103F03RB      10k±1%              3434                              */
/* ------------------------- DEFINIÇÕES PARA USB ---------------------------- */
#define         USB_FUNCSEL_PP           USB_HOST_PP
/* ----------------- DEFINIÇÕES PARA MENSAGENS PADRÕES ---------------------- */
#define         MENSAGEM_NORMAL         1
#define         MENSAGEM_EDICAO         2
#define         MENSAGEM_EXTRA          3
/*----------------------------------------------------------------------------*/
#define         MENSAGEM_MAX_TAMANHO    240
/* End user code. Do not edit comment generated here */
#endif