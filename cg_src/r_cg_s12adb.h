/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2012, 2018 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_cg_s12ad.h
* Version      : Applilet4 for RX111 V1.06.02.04 [18 May 2018]
* Device(s)    : R5F51115AxFL
* Tool-Chain   : IAR Systems ICCRX
* Description  : This file implements device driver for S12ADb module.
* Creation Date: 03/10/2018
***********************************************************************************************************************/
#ifndef S12ADB_H
#define S12ADB_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/*
    A/D control register (ADCSR)
*/
/* Group B scan end interrupt enable (GBADIE) */
#define _0000_AD_GBADI_DISABLE               (0x0000U) /* Disables GBADI interrupt generation upon group B scan
                                                          completion */
#define _0040_AD_GBADI_ENABLE                (0x0040U) /* Enables GBADI interrupt generation upon group B scan
                                                          completion */
/* Double trigger mode select (DBLE) */
#define _0000_AD_DBLTRIGGER_DISABLE          (0x0000U) /* Disable double trigger mode */
#define _0080_AD_DBLTRIGGER_ENABLE           (0x0080U) /* Enable double trigger mode */
/* Trigger select (EXTRG) */
#define _0000_AD_INTERNAL_TRIGGER            (0x0000U) /* A/D conversion started by synchronous trigger */
#define _0100_AD_EXTERNAL_TRIGGER            (0x0100U) /* A/D conversion started by asynchronous trigger */
/* Trigger start enable (TRGE) */
#define _0000_AD_TRIGGER_DISABLE             (0x0000U) /* A/D conversion by synchronous or asynchronous trigger disable */
#define _0200_AD_TRIGGER_ENABLE              (0x0200U) /* A/D conversion by synchronous or asynchronous trigger enable */
/* A/D conversion mode select (ADHSC) */
#define _0000_AD_NORMAL_CONVERSION           (0x0000U) /* Normal conversion */
#define _0400_AD_HIGH_SPEED_CONVERSION       (0x0400U) /* High-speed conversion */
/* Scan end interrupt enable (ADIE) */
#define _0000_AD_SCAN_END_INTERRUPT_DISABLE  (0x0000U) /* Disable S12ADI0 interrupt generation upon scan completion */
#define _1000_AD_SCAN_END_INTERRUPT_ENABLE   (0x1000U) /* Enable S12ADI0 interrupt generation upon scan completion */
/* Scan mode select (ADCS) */
#define _0000_AD_SINGLE_SCAN_MODE            (0x0000U) /* Single scan mode */
#define _2000_AD_GROUP_SCAN_MODE             (0x2000U) /* Group scan mode */
#define _4000_AD_CONTINUOUS_SCAN_MODE        (0x4000U) /* Continuous scan mode */
/* A/D conversion start (ADST) */
#define _0000_AD_CONVERSION_STOP             (0x0000U) /* Stop A/D conversion */
#define _8000_AD_CONVERSION_START            (0x8000U) /* Start A/D conversion */

/*
    A/D converted value addition count select register (ADADC)
*/
/* Addition Count Select ADC[1:0] */
#define _00_AD_1_TIME_CONVERSION             (0x00U)   /* 1-time conversion */
#define _01_AD_2_TIME_CONVERSION             (0x01U)   /* 2-time conversion */
#define _02_AD_3_TIME_CONVERSION             (0x02U)   /* 3-time conversion */
#define _03_AD_4_TIME_CONVERSION             (0x03U)   /* 4-time conversion */

/*
    A/D control extended register (ADCER)
*/
/* Automatic clearing enable (ACE) */
#define _0000_AD_AUTO_CLEARING_DISABLE       (0x0000U) /* Disable auto clearing */
#define _0020_AD_AUTO_CLEARING_ENABLE        (0x0020U) /* Enable auto clearing */
/* A/D data register format select (ADRFMT) */
#define _0000_AD_RIGHT_ALIGNMENT             (0x0000U) /* Right-alignment for data register format */
#define _8000_AD_LEFT_ALIGNMENT              (0x8000U) /* Left-alignment for data register format */

/*
    A/D start trigger select register (ADSTRGR)
*/
/* A/D conversion start trigger select for group B (TRSB) */
#define _0001_AD_TRSB_TRG0AN                 (0x0001U) /* TRGA input capture/compare match from MTU0 */
#define _0002_AD_TRSB_TRG0BN                 (0x0002U) /* TRGB input capture/compare match B from MTU0 */
#define _0003_AD_TRSB_TRGAN                  (0x0003U) /* TRGA input capture/compare match or MTU4.TCNT underflow in /
                                                          complementary PWM for MTU0~MTU4 */
#define _0004_AD_TRSB_TRG0EN                 (0x0004U) /* TRGE compare match from MTU0 */
#define _0005_AD_TRSB_TRG0FN                 (0x0005U) /* TRGF compare match from MTU0 */
#define _0006_AD_TRSB_TRG4AN                 (0x0006U) /* MTU4.TADCORA and MTU4.TCNT compare match */
#define _0007_AD_TRSB_TRG4BN                 (0x0007U) /* MTU4.TADCORB and MTU4.TCNT compare match */
#define _0008_AD_TRSB_TRG4ABN                (0x0008U) /* MTU4.TADCORA and MTU4.TCNT compare match and MTU4.TADCORB /
                                                          and MTU4.TCNT compare match */
#define _0009_AD_TRSB_ELC                    (0x0009U) /* trigger from ELC */
/* A/D conversion start trigger select for group A (TRSA) */
#define _0000_AD_TRSA_ADTRG0                 (0x0000U) /* A/D conversion start trigger pin */
#define _0100_AD_TRSA_TRG0AN                 (0x0100U) /* TRGA input capture/compare match from MTU0 */
#define _0200_AD_TRSA_TRG0BN                 (0x0200U) /* TRGB input capture/compare match B from MTU0 */
#define _0300_AD_TRSA_TRGAN                  (0x0300U) /* TRGA input capture/compare match or MTU4.TCNT underflow in /
                                                          complementary PWM for MTU0~MTU4 */
#define _0400_AD_TRSA_TRG0EN                 (0x0400U) /* TRGE compare match from MTU0 */
#define _0500_AD_TRSA_TRG0FN                 (0x0500U) /* TRGF compare match from MTU0 */
#define _0600_AD_TRSA_TRG4AN                 (0x0600U) /* MTU4.TADCORA and MTU4.TCNT compare match */
#define _0700_AD_TRSA_TRG4BN                 (0x0700U) /* MTU4.TADCORB and MTU4.TCNT compare match */
#define _0800_AD_TRSA_TRG4ABN                (0x0800U) /* MTU4.TADCORA and MTU4.TCNT compare match and MTU4.TADCORB /
                                                          and MTU4.TCNT compare match */
#define _0900_AD_TRSA_ELC                    (0x0900U) /* Trigger from ELC */

/*
    A/D converted extended input control register (ADEXICR)
*/
/* Temperature sensor output A/D converted value addition mode select (TSSAD) */
#define _0000_AD_TEMPS_ADDITION_DISABLE      (0x0000U) /* Temp sensor output converted value addition mode disabled */
#define _0001_AD_TEMPS_ADDITION_ENABLE       (0x0001U) /* Temp sensor output converted value addition mode enabled */
/* Internal reference voltage A/D-converted value addition mode select (OCSAD) */
#define _0000_AD_IREF_ADDITION_DISABLE       (0x0000U) /* Internal ref. voltage converted value addition mode disabled */
#define _0002_AD_IREF_ADDITION_ENABLE        (0x0002U) /* Internal ref. voltage converted value addition mode enabled */
/* Temperature sensor output A/D conversion select (TSS) */
#define _0000_AD_TEMPERATURE_SENSOR_DISABLE  (0x0000U) /* A/D conversion of temperature sensor output not performed */
#define _0100_AD_TEMPERATURE_SENSOR_ENABLE   (0x0100U) /* A/D conversion of temperature sensor output performed */
/* Internal reference voltage A/D conversion select (OCS) */
#define _0000_AD_IREF_CONVERSION_DISABLE     (0x0000U) /* A/D conversion of internal reference voltage not performed */
#define _0200_AD_IREF_CONVERSION_ENABLE      (0x0200U) /* A/D conversion of internal reference voltage performed */

/*
    Interrupt Source Priority Register n (IPRn)
*/
/* Interrupt Priority Level Select (IPR[3:0]) */
#define _00_AD_PRIORITY_LEVEL0               (0x00U)   /* Level 0 (interrupt disabled) */
#define _01_AD_PRIORITY_LEVEL1               (0x01U)   /* Level 1 */
#define _02_AD_PRIORITY_LEVEL2               (0x02U)   /* Level 2 */
#define _03_AD_PRIORITY_LEVEL3               (0x03U)   /* Level 3 */
#define _04_AD_PRIORITY_LEVEL4               (0x04U)   /* Level 4 */
#define _05_AD_PRIORITY_LEVEL5               (0x05U)   /* Level 5 */
#define _06_AD_PRIORITY_LEVEL6               (0x06U)   /* Level 6 */
#define _07_AD_PRIORITY_LEVEL7               (0x07U)   /* Level 7 */
#define _08_AD_PRIORITY_LEVEL8               (0x08U)   /* Level 8 */
#define _09_AD_PRIORITY_LEVEL9               (0x09U)   /* Level 9 */
#define _0A_AD_PRIORITY_LEVEL10              (0x0AU)   /* Level 10 */
#define _0B_AD_PRIORITY_LEVEL11              (0x0BU)   /* Level 11 */
#define _0C_AD_PRIORITY_LEVEL12              (0x0CU)   /* Level 12 */
#define _0D_AD_PRIORITY_LEVEL13              (0x0DU)   /* Level 13 */
#define _0E_AD_PRIORITY_LEVEL14              (0x0EU)   /* Level 14 */
#define _0F_AD_PRIORITY_LEVEL15              (0x0FU)   /* Level 15 (highest) */

/*
    PJ6 Pin Function Control Registers (PJ6PFS)
*/
/* Analog Function Select (ASEL) */
#define _00_AD_REF_HIGH_AVCC0                (0x00U)   /* AVCC0 pin is selected as high-electric potential */
#define _80_AD_REF_HIGH_VREFH0               (0x80U)   /* VREFH0 pin is selected as low-electric potential */

/*
    PJ7 Pin Function Control Registers (PJ7PFS)
*/
/* Analog Function Select (ASEL) */
#define _00_AD_REF_LOW_AVSS0                 (0x00U)   /* AVSS0 pin is selected as low-electric potential */
#define _80_AD_REF_LOW_VREFL0                (0x80U)   /* VREFL0 pin is selected as high-electric potential */

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define _0800_AD_CHANNEL_SELECT_A            (0x0800U) /* Channel select register A value */
#define _0000_AD_CHANNEL_SELECT_B            (0x0000U) /* Channel select register B value */
#define _0000_AD_VALUE_ADDITION_SELECT       (0x0000U) /* Converted value addition mode select register */
#define _14_AD_SAMPLING_STATE_L              (0x14U) /* AN008 to AN015 sampling state register value */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef enum
{
    ADCHANNEL0, ADCHANNEL1, ADCHANNEL2, ADCHANNEL3, ADCHANNEL4, ADCHANNEL6,
    ADCHANNEL8, ADCHANNEL9, ADCHANNEL10, ADCHANNEL11, ADCHANNEL12, ADCHANNEL13,
    ADCHANNEL14, ADCHANNEL15, ADTEMPSENSOR, ADINTERREFVOLT, ADDATADUPLICATION
} ad_channel_t;
/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_S12AD_Create(void);
void R_S12AD_Start(void);
void R_S12AD_Stop(void);
void R_S12AD_Get_ValueResult(ad_channel_t channel, uint16_t * const buffer);

/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif
