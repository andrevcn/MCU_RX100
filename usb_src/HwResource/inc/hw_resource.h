/******************************************************************************
* File Name     : hw_resource.h
* Version       : 2.10
* Description   : RX111 MCU/TargetBoard define
*******************************************************************************
* History   : DD.MM.YYYY Version Description
*           : 01.08.2013 2.10    Basic Mini FW Ver2.10 Release
******************************************************************************/
#ifndef __HW_RESOURCE_H__
#define __HW_RESOURCE_H__

/******************************************************************************
Constant macro definitions
******************************************************************************/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Select the driver by the peripheral function integrated into the MCU.
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
//#define     USB_LCD_ENABLE
//#define     USB_LED_ENABLE
//#define     USB_KEY_ENABLE

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
/* variables */

/* MCU & USB functions */
extern void     usb_cpu_target_init(void);          /* Target initialize */
extern void     usb_cpu_int_enable(void);           /* Enable USB interrupt */
extern void     usb_cpu_int_disable(void);          /* Disable USB interrupt */
extern void     usb_cpu_delay_xms(uint16_t time);   /* Delay */
extern void     usb_cpu_delay_1us(uint16_t time);

/******************************************************************************
Macro definitions
******************************************************************************/
#define     PORT_LOW        0
#define     PORT_HIGH       1
#define     PORT_OUT_MODE   1
#define     PORT_IN_MODE    0
/************* 48pin Setting *************/
#define     PIOR_VALUE      0x00        /* PIOR0 is Don't care */
#define     ADPC_VALUE      0x04        /* P20-P22is ad input */


/* LCD DEFINE */
#define     LCD_MAXCHARA    8
#define     LCD_POS_U0      0
#define     LCD_POS_U1      1
#define     LCD_POS_U2      2
#define     LCD_POS_U3      3
#define     LCD_POS_U4      4
#define     LCD_POS_U5      5
#define     LCD_POS_U6      6
#define     LCD_POS_U7      7
#define     LCD_POS_D0      LCD_POS_U0+LCD_MAXCHARA
#define     LCD_POS_D1      LCD_POS_U1+LCD_MAXCHARA
#define     LCD_POS_D2      LCD_POS_U2+LCD_MAXCHARA
#define     LCD_POS_D3      LCD_POS_U3+LCD_MAXCHARA
#define     LCD_POS_D4      LCD_POS_U4+LCD_MAXCHARA
#define     LCD_POS_D5      LCD_POS_U5+LCD_MAXCHARA
#define     LCD_POS_D6      LCD_POS_U6+LCD_MAXCHARA
#define     LCD_POS_D7      LCD_POS_U7+LCD_MAXCHARA


/* ADC DEFINE */


/* LED DEFINE */
#define     LED_OFF         0
#define     LED_ON          1
#define     LED_POS0        1
#define     LED_POS1        2
#define     LED_POS2        4
#define     LED_POS3        8


/* KEY DEFINE */
#define NO_PUSH             0x00
#define SW1_PUSH            0x01
#define SW2_PUSH            0x02
#define SW3_PUSH            0x03
#define SW4_PUSH            0x04

#endif  /* __HW_RESOURCE_H__ */
/******************************************************************************
End Of File
******************************************************************************/
