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
* File Name    : r_cg_rtc.h
* Version      : Applilet4 for RX111 V1.00.00.05 [06 Jun 2013]
* Device(s)    : R5F51115AxFL
* Tool-Chain   : IAR Systems icc78k0r
* Description  : This file implements device driver for RTC module.
* Creation Date: 6/18/2014
***********************************************************************************************************************/
#ifndef RTC_H
#define RTC_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/*
    RTC Control Register 1 (RCR1)
*/
/* Alarm Interrupt Enable (AIE) */
#define _00_RTC_ALARM_INT_DISABLE                   (0x00U)
#define _01_RTC_ALARM_INT_ENABLE                    (0x01U)
/* Carry Interrupt Enable (CIE) */
#define _00_RTC_CARRY_INT_DISABLE                   (0x00U)
#define _02_RTC_CARRY_INT_ENABLE                    (0x02U)
/* Periodic Interrupt Enable (PIE) */
#define _00_RTC_PERIOD_INT_DISABLE                  (0x00U)
#define _04_RTC_PERIOD_INT_ENABLE                   (0x04U)
/* RTCOUT Output Select (RTCOS) */
#define _00_RTC_RTCOUT_FREQ_1HZ                     (0x00U)
#define _08_RTC_RTCOUT_FREQ_64HZ                    (0x08U)
/* Periodic Interrupt Select (PES[3:0]) */
#define _00_RTC_PERIODIC_INT_PERIOD_DISABLE         (0x00U)
#define _60_RTC_PERIODIC_INT_PERIOD_1_256           (0x60U)
#define _70_RTC_PERIODIC_INT_PERIOD_1_128           (0x70U)
#define _80_RTC_PERIODIC_INT_PERIOD_1_64            (0x80U)
#define _90_RTC_PERIODIC_INT_PERIOD_1_32            (0x90U)
#define _A0_RTC_PERIODIC_INT_PERIOD_1_16            (0xA0U)
#define _B0_RTC_PERIODIC_INT_PERIOD_1_8             (0xB0U)
#define _C0_RTC_PERIODIC_INT_PERIOD_1_4             (0xC0U)
#define _D0_RTC_PERIODIC_INT_PERIOD_1_2             (0xD0U)
#define _E0_RTC_PERIODIC_INT_PERIOD_1               (0xE0U)
#define _F0_RTC_PERIODIC_INT_PERIOD_2               (0xF0U)

/*
    RTC Control Register 2 (RCR2)
*/
/* Start (START) */
#define _00_RTC_RCR2_START_STOP                     (0x00U)
#define _01_RTC_RCR2_START_START                    (0x01U)
/* RTC Software Reset (RESET) */
#define _00_RTC_RCR2_RESET_READY_READONLY           (0x00U)
#define _02_RTC_RCR2_RESET                          (0x02U)
/* 30-Second Adjustment (ADJ30) */
#define _00_RTC_30SEC_ADJUSTMENT_READY_READONLY     (0x00U)
#define _04_RTC_30SEC_ADJUSTMENT                    (0x04U)
/* RTCOUT Output Enable (RTCOE) */
#define _00_RTC_RTCOUT_OUTPUT_DISABLE               (0x00U)
#define _08_RTC_RTCOUT_OUTPUT_ENABLE                (0x08U)
/* Automatic Adjustment Enable (AADJE) */
#define _00_RTC_AUTO_ADJUSTMENT_DISABLE             (0x00U)
#define _10_RTC_AUTO_ADJUSTMENT_ENABLE              (0x10U)
/* Automatic Adjustment Period Select (AADJP) */
#define _00_RTC_AUTO_ADJUSTMENT_PERIOD_1MIN         (0x00U)
#define _20_RTC_AUTO_ADJUSTMENT_PERIOD_10SEC        (0x20U)
#define _00_RTC_AUTO_ADJUSTMENT_PERIOD_32SEC        (0x00U)
#define _20_RTC_AUTO_ADJUSTMENT_PERIOD_8SEC         (0x20U)
/* Hours Mode (HR24) */
#define _00_RTC_HOUR_MODE_12                        (0x00U)
#define _40_RTC_HOUR_MODE_24                        (0x40U)
/* Count Mode Select (CNTMD) */
#define _00_RTC_COUNT_MODE_CAL                      (0x00U)
#define _80_RTC_COUNT_MODE_BIN                      (0x80U)

/*
    RTC Control Register (RCR3)
*/
/* Sub-clock Control (RTCEN) */
#define _00_RTC_SUBCLOCK_DISABLE                    (0x00U)
#define _01_RTC_SUBCLOCK_ENABLE                     (0x01U)
/* Sub-clock Oscillator Drive Capacity Control (RTCDV[1:0]) */
#define _00_RTC_SUBCLOCK_DRIVE_MID                  (0x00U)
#define _02_RTC_SUBCLOCK_DRIVE_HIGH                 (0x02U)
#define _04_RTC_SUBCLOCK_DRIVE_LOW                  (0x04U)

/*
    Timer Error Adjustment Register (RADJ)
*/
/* Plus-Minus (PMADJ[1:0]) */
#define _40_RTC_TIMER_ERROR_ADJUST_PLUS             (0x40U)
#define _80_RTC_TIMER_ERROR_ADJUST_MINUS            (0x80U)

/*
    Hour Counter (RHRCNT)
*/
/* PM */
#define _00_RTC_HOUR_ATTRIB_AM                      (0x00U)
#define _40_RTC_HOUR_ATTRIB_PM                      (0x40U)

/*
    Second Alarm Register (RSECAR)
*/
/* ENB */
#define _00_RTC_ALARM_SECOND_DISABLE                (0x00U)
#define _80_RTC_ALARM_SECOND_ENABLE                 (0x80U)

/*
    Minute Alarm Register (RMINAR)
*/
/* ENB */
#define _00_RTC_ALARM_MINUTE_DISABLE                (0x00U)
#define _80_RTC_ALARM_MINUTE_ENABLE                 (0x80U)

/*
    Hour Alarm Register (RHRAR)
*/
/* ENB */
#define _00_RTC_ALARM_HOUR_DISABLE                  (0x00U)
#define _80_RTC_ALARM_HOUR_ENABLE                   (0x80U)
/* PM */
#define _00_RTC_ALARM_HOUR_ATTRIB_AM                (0x00U)
#define _40_RTC_ALARM_HOUR_ATTRIB_PM                (0x40U)

/* 
    Day-of-Week Alarm Register (RWKAR)
*/
/* ENB */
#define _00_RTC_ALARM_WEEK_DISABLE                  (0x00U)
#define _80_RTC_ALARM_WEEK_ENABLE                   (0x80U)

/*
    Date Alarm Register (RDAYAR)
*/
/* ENB */
#define _00_RTC_ALARM_DAY_DISABLE                   (0x00U)
#define _80_RTC_ALARM_DAY_ENABLE                    (0x80U)

/*
    Month Alarm Register (RMONAR)
*/
/* ENB */
#define _00_RTC_ALARM_MONTH_DISABLE                 (0x00U)
#define _80_RTC_ALARM_MONTH_ENABLE                  (0x80U)

/*
    Year Alarm Enable Register (RYRAREN)
*/
/* ENB */
#define _00_RTC_ALARM_YEAR_DISABLE                  (0x00U)
#define _80_RTC_ALARM_YEAR_ENABLE                   (0x80U)
/*
    Interrupt Priority
*/
#define _00_RTC_PRIORITY_LEVEL0                     (0x00U)
#define _01_RTC_PRIORITY_LEVEL1                     (0x01U)
#define _02_RTC_PRIORITY_LEVEL2                     (0x02U)
#define _03_RTC_PRIORITY_LEVEL3                     (0x03U)
#define _04_RTC_PRIORITY_LEVEL4                     (0x04U)
#define _05_RTC_PRIORITY_LEVEL5                     (0x05U)
#define _06_RTC_PRIORITY_LEVEL6                     (0x06U)
#define _07_RTC_PRIORITY_LEVEL7                     (0x07U)
#define _08_RTC_PRIORITY_LEVEL8                     (0x08U)
#define _09_RTC_PRIORITY_LEVEL9                     (0x09U)
#define _0A_RTC_PRIORITY_LEVEL10                    (0x0AU)
#define _0B_RTC_PRIORITY_LEVEL11                    (0x0BU)
#define _0C_RTC_PRIORITY_LEVEL12                    (0x0CU)
#define _0D_RTC_PRIORITY_LEVEL13                    (0x0DU)
#define _0E_RTC_PRIORITY_LEVEL14                    (0x0EU)
#define _0F_RTC_PRIORITY_LEVEL15                    (0x0FU)

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define _00000370_RTC_RTCSCLK_STABLE_CYCLE          (0x00000370UL)
#define _000124F9_RTC_1_64_SEC_CYCLE              (0x000124F9UL)
#define _15_RTC_COUNT_SECOND_VALUE                  (0x15U)
#define _15_RTC_COUNT_MINUTE_VALUE                  (0x15U)
#define _15_RTC_COUNT_HOUR_VALUE                    (0x15U)
#define _03_RTC_COUNT_WEEK_VALUE                    (0x03U)
#define _18_RTC_COUNT_DAY_VALUE                     (0x18U)
#define _06_RTC_COUNT_MONTH_VALUE                   (0x06U)
#define _0014_RTC_COUNT_YEAR_VALUE                  (0x0014U)

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef struct 
{
    uint8_t rseccnt;
    uint8_t rmincnt;
    uint8_t rhrcnt;
    uint8_t rdaycnt;
    uint8_t rwkcnt;
    uint8_t rmoncnt;
    uint16_t ryrcnt;
} rtc_calendarcounter_value_t;

typedef struct
{
    uint8_t sec_enb;
    uint8_t min_enb;
    uint8_t hr_enb;
    uint8_t day_enb;
    uint8_t wk_enb;
    uint8_t mon_enb;
    uint8_t yr_enb;
} rtc_calendar_alarm_enable_t;

typedef struct
{
    uint8_t rsecar;
    uint8_t rminar;
    uint8_t rhrar;
    uint8_t rdayar;
    uint8_t rwkar;
    uint8_t rmonar;
    uint16_t ryrar;
} rtc_calendar_alarm_value_t;

typedef enum
{
    PES_1_256_SEC = 0x60U,
    PES_1_128_SEC = 0x70U,
    PES_1_64_SEC = 0x80U,
    PES_1_32_SEC = 0x90U,
    PES_1_16_SEC = 0xA0U,
    PES_1_8_SEC = 0xB0U,
    PES_1_4_SEC = 0xC0U,
    PES_1_2_SEC = 0xD0U,
    PES_1_SEC = 0xE0U,
    PES_2_SEC = 0xF0U
} rtc_int_period_t;

typedef enum
{
    RTCOUT_1HZ,
    RTCOUT_64HZ
} rtc_rtcout_period_t;

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_RTC_Create(void);
void R_RTC_Start(void);
void R_RTC_Stop(void);
void R_RTC_Restart(rtc_calendarcounter_value_t counter_write_val);
void R_RTC_Get_CalendarCounterValue(rtc_calendarcounter_value_t * const counter_read_val);
void R_RTC_Set_CalendarCounterValue(rtc_calendarcounter_value_t counter_write_val);

/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif