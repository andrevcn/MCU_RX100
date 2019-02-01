/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer *
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name     : r_usb_hhid_apl.c
* Version       : 2.10
* Description   : USB Host HID application function code.
*******************************************************************************/
/*******************************************************************************
* History   : DD.MM.YYYY Version Description
*           : 01.08.2013 2.10    Basic Mini FW Ver2.10 Release
*******************************************************************************/


/******************************************************************************
Section definitions
******************************************************************************/
#ifdef  R8CUSB
#pragma section program P_smpl
#pragma section bss B_smpl
#pragma section data R_smpl
#pragma section rom C_smpl
#endif
#ifdef  RL78USB
#endif
#ifdef  RL78USB_SECTION
#pragma section @@CODE   P_HID
#pragma section @@BASE   T_HID
#pragma section @@DATA   B_HID
#endif
#ifdef  __RX
#pragma section = "_smpl"
#endif


/******************************************************************************
Includes <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_ctypedef.h"             /* Type define */
#include "r_usb_ckernelid.h"            /* Kernel ID definition */
#include "r_usb_usrconfig.h"            /* System definition, USB-FW Library Header */
#include "r_usb_cdefusbip.h"            /* USB-FW Library Header */
#include "r_usb_usbip.h"                /* Definition of the USB register & bit define */
#include "r_usb_api.h"                  /* USB API public function header */
#include "r_usb_hhid_usrcfg.h"          /* HID class specification of user configuration */
#include "r_usb_hhid_define.h"          /* HID class define */
#include "r_usb_hhid_api.h"
#include "hw_resource.h"
#include "r_user_hwrctrl.h"

/******************************************************************************
Macro definitions
******************************************************************************/
/* Switch define */
#define USB_HHID_SW_OFF                     NO_PUSH     /* No push */
#define USB_HHID_SW2_ON                     SW2_PUSH    /* SW2 push */
#define USB_HHID_SW3_ON                     SW3_PUSH    /* SW3 push */

/* LCD Display Position */
#define USB_HHID_LCD_DISP_POSITION_LINE1    LCD_POS_U0  /* Line 1 */
#define USB_HHID_LCD_DISP_POSITION_LINE2    LCD_POS_D0  /* Line 2 */
#define USB_HHID_DISP_X_AXIS_POSITION       LCD_POS_U0  /* Mouse X axis */
#define USB_HHID_DISP_Y_AXIS_POSITION       LCD_POS_U4  /* Mouse Y axis */
#define USB_HHID_DISP_KEY_POSITION          LCD_POS_D6  /* Keyborad Input Key */

/* LED Position for USB Mouse */
#define USB_HHID_LED_POS_LEFT_BTN           LED_POS0    /* Left Button */
#define USB_HHID_LED_POS_RIGHT_BTN          LED_POS1    /* Right Button */
#define USB_HHID_LED_POS_WHEEL_BTN          LED_POS2    /* Wheel Button */

/* Mouse Button Defines */
#define USB_HHID_LEFT_BUTTON                0x01        /* Left Button */
#define USB_HHID_RIGHT_BUTTON               0x02        /* Right Button */
#define USB_HHID_WHEEL_BUTTON               0x04        /* Wheel Button */

/* USB Mouse no movement */
#define USB_HHID_MSE_NOT_MOVE               0x00

/* Mouse data Store Position Index */
#define USB_HHID_MSE_BTN_DATA_INDEX         0x00        /* Input Key */
#define USB_HHID_MSE_X_DATA_INDEX           0x01        /* X-axis */
#define USB_HHID_MSE_Y_DATA_INDEX           0x02        /* Y-axis */

#define USB_HHID_KBD_TRN_LEN                0x08        /* Keyboard mode USB Transaction Length */

#define USB_HHID_KBD_NOT_PUSH               0x00        /* USB Keyboard no key input */

/* Keyboard data Store Position Index */
#define USB_HHID_KBD_MODIFIERKEY_INDEX      0x00        /* Modifier Keys */
#define USB_HHID_KBD_RESERVED_INDEX         0x01        /*  */
#define USB_HHID_KBD_KEYCODE1_INDEX         0x02        /* Input Key */
#define USB_HHID_KBD_KEYCODE2_INDEX         0x03        /*  */
#define USB_HHID_KBD_KEYCODE3_INDEX         0x04        /*  */
#define USB_HHID_KBD_KEYCODE4_INDEX         0x05        /*  */
#define USB_HHID_KBD_KEYCODE5_INDEX         0x06        /*  */
#define USB_HHID_KBD_KEYCODE6_INDEX         0x07        /*  */

/* USB HID Keyboard Usage ID */
#define USB_HHID_KBD_CODE_2                 0x03        /* 2 */
#define USB_HHID_KBD_CODE_1                 0x1E        /* 1 */
#define USB_HHID_KBD_CODE_0                 0x27        /* 0 */
#define USB_HHID_KBD_CODE_1AUX              0x59        /* 1 - aux */
#define USB_HHID_KBD_CODE_0AUX              0x62        /* 0 - aux */
#define USB_HHID_KBD_CODE_FUNCTION          0x45        /* Function 12 */
#define USB_HHID_KBD_CODE_ENTER             0x28        /* '\n' */
#define USB_HHID_KBD_CODE_ESC               0x29        /* ESC */
#define USB_HHID_KBD_CODE_BS                0x2A        /* 'BS' */
#define USB_HHID_KBD_CODE_SPACE             0x2C        /* ' ' */
#define USB_HHID_KBD_CODE_HYPHEN            0x2D        /* '-' */
#define USB_HHID_KBD_CODE_COLON             0x34        /* ':' */
#define USB_HHID_KBD_CODE_DOT               0x37        /* '.' */
#define USB_HHID_KBD_CODE_DIAGONAL          0x38        /* '/' */
#define USB_HHID_KBD_CODE_DEL               0X4c        /* DELETE */
#define USB_HHID_KBD_CODE_UNDERSCORE        0x87        /* '_' */
#define USB_HHID_KBD_CODE_YEN               0x89        /* '\' */
#define USB_HHID_KBD_CODE_NUMLOCK           0x53        /* Num Lock */
#define USB_HHID_KBD_CODE_CAPSLOCK          0x39        /* Caps Lock */
#define USB_HHID_KBD_CODE_SCROLLLOCK        0x47        /* Scroll Lock */
#define USB_HHID_KBD_CODE_DIRUP             0x52        /* Direcional Cima */

/* Keycode to ASCII code offset */
#define USB_HHID_KEYCODE_TO_ASCII_OFFSET0       0x3D        /* 'A'-'Z' */
#define USB_HHID_KEYCODE_TO_ASCII_OFFSET1       0x13        /* '1'-'9' */
#define USB_HHID_KEYCODE_TO_ASCII_OFFSET2       0x09        /* '0' */
#define USB_HHID_KEYCODE_TO_ASCII_OFFSET3       0x5D        /* 'a'-'z' */
#define USB_HHID_KEYCODE_TO_ASCII_OFFSET1AUX    0x28        /* '1'-'9' - AUX */
#define USB_HHID_KEYCODE_TO_ASCII_OFFSET2AUX    0x32        /* '0' - AUX*/
#define USB_HHID_KEYCODE_TO_ASCII_FOFFSET       0x39        /* Function Offset */
#define USB_HHID_ASCII_BS                       0x08        /* BackSpace */

/* Set Report (Keyborad LED) */
#define USB_HHID_NUM_LOCK                   0x01
#define USB_HHID_CAPS_LOCK                  0x02
#define USB_HHID_SCROLL_LOCK                0x04
#define USB_HHID_KBD_LED_CLEAR              0x00

/* ----- Host HID Application State ----- */
#define USB_HHID_APL_CLOSE                  0x81        /* Close State */
#define USB_HHID_APL_MODE_KBD               0x90        /* Keyboard Mode State */
#define USB_HHID_APL_MODE_MSE               0xA0        /* Mouse Mode State */
/* Host HID Application State Offset */
#define USB_HHID_APL_OFFSET_OPEN            1           /* Open State */
#define USB_HHID_APL_OFFSET_RX_WAIT         2           /* Data receive wait State */
#define USB_HHID_APL_OFFSET_RX              3           /* Data receive complete State */
#define USB_HHID_APL_OFFSET_SUSPEND         4           /* Suspend */
#define USB_HHID_APL_OFFSET_RESUME          5           /* Resume */
#define USB_HHID_APL_OFFSET_CTL_END         6           /* control transfer complete */

#define USB_HHID_APL_IDLE                   0
#define USB_HHID_APL_INIT                   USB_HHID_APL_OFFSET_OPEN
#define USB_HHID_APL_KBD_OPEN               (USB_HHID_APL_MODE_KBD + USB_HHID_APL_OFFSET_OPEN)
#define USB_HHID_APL_MSE_OPEN               (USB_HHID_APL_MODE_MSE + USB_HHID_APL_OFFSET_OPEN)
#define USB_HHID_APL_KBD_RX_WAIT            (USB_HHID_APL_MODE_KBD + USB_HHID_APL_OFFSET_RX_WAIT)
#define USB_HHID_APL_MSE_RX_WAIT            (USB_HHID_APL_MODE_MSE + USB_HHID_APL_OFFSET_RX_WAIT)
#define USB_HHID_APL_KBD_RX                 (USB_HHID_APL_MODE_KBD + USB_HHID_APL_OFFSET_RX)
#define USB_HHID_APL_MSE_RX                 (USB_HHID_APL_MODE_MSE + USB_HHID_APL_OFFSET_RX)
#define USB_HHID_APL_KBD_SUSPEND            (USB_HHID_APL_MODE_KBD + USB_HHID_APL_OFFSET_SUSPEND)
#define USB_HHID_APL_MSE_SUSPEND            (USB_HHID_APL_MODE_MSE + USB_HHID_APL_OFFSET_SUSPEND)
#define USB_HHID_APL_KBD_RESUME             (USB_HHID_APL_MODE_KBD + USB_HHID_APL_OFFSET_RESUME)
#define USB_HHID_APL_MSE_RESUME             (USB_HHID_APL_MODE_MSE + USB_HHID_APL_OFFSET_RESUME)
#define USB_HHID_APL_KBD_CTL_END            (USB_HHID_APL_MODE_KBD + USB_HHID_APL_OFFSET_CTL_END)
#define USB_HHID_APL_MSE_CTL_END            (USB_HHID_APL_MODE_MSE + USB_HHID_APL_OFFSET_CTL_END)

/* Application State Mode Mask */
#define USB_HHID_APL_MODE_MASK               0xf0

/* Definições relacionadas ao bits do registrados "usb_shhid_SpecialKeys" */
#define NUM_LOCK                        (1<<0)
#define CAPS_LOCK                       (1<<1)
#define SCROLL_LOCK                     (1<<2)

#define RD_USB_SPCKEY(x)             (usb_shhid_SpecialKeys & x)
#define SET_USB_SPCKEY(x)            (usb_shhid_SpecialKeys |= x)
#define CLR_USB_SPCKEY(x)            (usb_shhid_SpecialKeys &= ~x)

/* Definições relacionadas ao registrador usb_shhid_ModifierKeys */
#define RIGHT_SHIFT                     0x20
#define LEFT_SHIFT                      0x02

#define RD_USB_MODIFKEY(x)             (usb_shhid_ModifierKeys & x)
#define SET_USB_MODIFKEY(x)            (usb_shhid_ModifierKeys |= x)
#define CLR_USB_MODIFKEY(x)            (usb_shhid_ModifierKeys &= ~x)
/******************************************************************************
Typedef definitions
******************************************************************************/
/* There is no typedef definition. */


/******************************************************************************
Private global variables and functions
******************************************************************************/
/* variables */
USB_STATIC uint8_t      usb_shhid_smpl_trn_data[USB_HHID_KBD_TRN_LEN];    /* Receive data for HID device. */
USB_STATIC uint8_t      usb_shhid_set_report_data[USB_HHID_KBD_TRN_LEN];
USB_STATIC uint16_t     usb_shhid_apl_function;     /* Application seaquence */
USB_STATIC usb_addr_t   usb_shhid_smpl_devaddr;     /* Hid Device address */
USB_STATIC uint8_t      usb_shhid_KbdKey;           /* Keyboard input values */
USB_STATIC uint8_t      usb_shhid_FunctionKey;      /* Keyboard Function values */
USB_STATIC uint8_t      usb_shhid_ModifierKeys;     /* Modifier Keys */
USB_STATIC uint16_t     usb_shhid_report_length;    /* Report Packet Length */
USB_STATIC uint16_t     usb_shhid_active;           /* Host HID Driver Status (Active/Not Active) */
USB_STATIC usb_port_t   g_usb_hsmpl_devport;        /* Hid Device connected port */
USB_STATIC USB_HHID_CLASS_REQUEST_PARM_t    g_usb_hsmpl_request;
USB_STATIC uint8_t      usb_shhid_SpecialKeys;

/* functions */
USB_STATIC void  usb_hapl_registration(void);
USB_STATIC void  usb_hhid_class_check(uint8_t **table);
USB_STATIC void  usb_hsmpl_device_state(uint16_t data, uint16_t state);
USB_STATIC void  usb_hhid_smpl_data_trans_result(usb_utr_t *mess);
USB_STATIC void  usb_hhid_smpl_kbd_data(uint8_t *data);
USB_STATIC void  usb_hhid_smp_status_set(uint16_t status);
USB_STATIC void  usb_hhid_smpl_kbd_led_ctl(uint16_t mode);
USB_STATIC void  usb_hhid_smpl_set_report(uint8_t *p_data, uint16_t length);
USB_STATIC void  usb_hhid_smpl_get_report(uint8_t *p_data, uint16_t length);
USB_STATIC void  usb_hhid_smpl_set_protocol(uint8_t protocol);
USB_STATIC void  usb_hsmpl_class_result(usb_utr_t *mess);
USB_STATIC void  usb_hhid_smpl_get_report_result(void);
USB_STATIC void  usb_hhid_smpl_kbd_led_ctl_result(void);
#ifdef  CLASS_REQUEST_SAMPLE
USB_STATIC void  usb_hhid_smpl_set_idle(uint16_t duration);
USB_STATIC void  usb_hhid_smpl_get_idle(uint16_t *idle_rate);
USB_STATIC void  usb_hhid_smpl_get_hid_descriptor(uint8_t *p_data);
USB_STATIC void  usb_hhid_smpl_get_report_descriptor(uint8_t *p_data, uint16_t length);
USB_STATIC void  usb_hhid_smpl_get_physical_descriptor(uint8_t *p_data, uint16_t length, uint8_t index);
USB_STATIC void  usb_hhid_smpl_get_protocol(uint8_t *protocol);
#endif  /* CLASS_REQUEST_SAMPLE */

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
/* variables */

/* functions */
void        usb_hsmpl_main_init(void);
void        usb_hhid_MainTask(void);
#include "r_cg_rtc.h"


/* variables -------------------------------------------------------------------------------------*/
extern uint8_t         AjusteHora;
extern uint8_t         AjusteMinuto;
extern uint8_t         AjusteDia;
extern uint8_t         AjusteMes;
extern uint32_t       Sys_FlowCounter;
uint8_t               trocou_parametro = 0;
extern unsigned char Sys_TachC, Sys_TachD, Sys_TachU;
extern unsigned int FatorK;
extern          uint16_t        Sys_TypeOfMsgOnDisplay;
extern          uint16_t        Delay_mS(uint16_t* value, uint16_t time);
extern          uint16_t        Sys_EditPointMsg;
extern          uint32_t        Sys_DisplayFlags; /* Flags de controle do display */
extern          uint32_t        Sys_ActualDisplayMessage; /* Carrega o valor da mensagem atual sendo mostrada (0 -> nenhuma. 1-8 -> mensagens)*/
extern          uint32_t        Sys_LastDisplayMessage; /* Carrega o valor da última mensagem mostrada */
extern          uint16_t        Sys_MessageLoadControlPointer;
uint16_t        Sys_FunctionSelected;
extern          uint32_t        Sys_MessagePointer;
extern          uint16_t        Sys_InfoDelay;
extern          uint16_t        Sys_InfoDelayTime;
extern          uint16_t        Sys_HoraEmMinutos;
uint16_t        Sys_AccentBuffer;
extern          uint8_t         msg_edicao[240];

const uint8_t ScanCodes[66][2] = {
  {'a','A'}, /* 0 */
  {'b','B'},
  {'c','C'},
  {'d','D'},
  {'e','E'},
  {'f','F'},
  {'g','G'},
  {'h','H'},
  {'i','I'},
  {'j','J'},
  {'k','K'},
  {'l','L'},
  {'m','M'},
  {'n','N'},
  {'o','O'},
  {'p','P'},
  {'q','Q'},
  {'r','R'},
  {'s','S'},
  {'t','T'},
  {'u','U'},
  {'v','V'},
  {'w','W'},
  {'x','X'},
  {'y','Y'},
  {'z','Z'}, /* 25 */

  {'1','!'}, /* 26 */
  {'2','@'},
  {'3','#'},
  {'4','$'},
  {'5','%'},
  {'6','¨'},
  {'7','&'},
  {'8','*'},
  {'9','('},
  {'0',')'}, /* 35 */

  {' ',0}, /* 36 */
  {'-','_'},
  {'=','+'},
  {'´','`'},
  {'[','{'},
  {92,'|'},
  {']','}'},
  {'ç','Ç'},
  {'~','^'},
  {39,'"'},
  {',','<'},
  {'.','>'},
  {';',':'}, /* 48 */

  {'/',0}, /* 49 */
  {'*',0},
  {'-',0},
  {'+',0}, /* 52 */

  {'1',0}, /* 53 */
  {'2',0},
  {'3',0},
  {'4',0},
  {'5',0},
  {'6',0},
  {'7',0},
  {'8',0},
  {'9',0},
  {'0',0},
  {'.',0},
  {92,'|'}, /* 64 */

  {'/','?'}, /* 65 */
};

/* functions -------------------------------------------------------------------------------------*/
extern void     usb_smpl_lcd_string(uint8_t pos, uint8_t *str);
extern void     usb_smpl_led_position(uint8_t ledpos, uint8_t data);
extern uint8_t  usb_smpl_key_input(void);
extern void     usb_smpl_set_suspend_flag(uint8_t data);
extern void FDL_WriteControl (void);
extern void SaveMsg(uint8_t ptrMsg);
extern void ZeraEfeito(void);
extern void Sys_ConvertASCIItoMatriz(uint32_t MsgNumber);


/******************************************************************************
Function Name   : usb_hsmpl_main_init
Description     : Initialize process
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hsmpl_main_init(void)
{
  /* Target board initialize */
  usb_cpu_target_init();

  /* USB-IP initialized */
  R_usb_hstd_ChangeDeviceState(USB_DO_INITHWFUNCTION);

  /* HCD driver open & registratuion */
  R_usb_hstd_HcdOpen();                           /* HCD task, MGR task open */
  usb_hapl_registration();                        /* Sample driver registration */

  /* Scheduler initialized */
}

/******************************************************************************
Function Name   : usb_hhid_MainTask
Description     : Host HID sample application main process
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hhid_MainTask(void)
{
  uint8_t  key_data;
  uint8_t  protocol;

  key_data = USB_HHID_SW2_ON;

  /* Host HID task status check */
  if ((USB_NO == usb_shhid_active) && (USB_HHID_APL_CLOSE == usb_shhid_apl_function))
  {
    /* Application seaquence Keyboard Mode open set */
    //quando tira fora o teclado
    usb_shhid_apl_function = USB_HHID_APL_KBD_OPEN;
  }

  if(USB_NO == usb_shhid_active)
  {
    key_data = NO_PUSH;
  }

  /* Branch HHID Application State */
  switch (usb_shhid_apl_function)
  {

    /* Close Statue */
  case USB_HHID_APL_CLOSE:
    break;

  case USB_HHID_APL_INIT:
    /* Branch bInterfaceProtocol - Interface Descriptor */
    protocol = R_usb_hhid_get_hid_protocol();
    switch (protocol)
    {
      /* Keyboard */
    case    USB_HID_IFPRO_KBD:
      /* Keyboard Open Message Display */
      /* Application seaquence Keyboard Mode open set */
      usb_shhid_apl_function = USB_HHID_APL_KBD_OPEN;
      /* Num Lock On */
      //usb_hhid_smpl_kbd_led_ctl(USB_HHID_NUM_LOCK);
      usb_hhid_smpl_kbd_led_ctl(USB_HHID_CAPS_LOCK);
      SET_USB_SPCKEY(CAPS_LOCK);
      break;
      /* Mouse */
    case    USB_HID_IFPRO_MSE:
      /* Mouse Open Message Display */
      /* Application seaquence Mouse Mode open set */
      usb_shhid_apl_function = USB_HHID_APL_MSE_OPEN;
      break;
    default:
      break;
    }
    break;

    /* Keyboard Mode Open State & Mouse Mode Open State */
  case USB_HHID_APL_KBD_OPEN:
    /* continue */
  case USB_HHID_APL_MSE_OPEN:
    if (key_data == USB_HHID_SW2_ON)
    {
      /* Keyborad Mode Boot Protocol set */
      if (usb_shhid_apl_function == USB_HHID_APL_KBD_OPEN)
      {
        /* Protocol:Boot */ //ver aqui
        usb_hhid_smpl_set_protocol(USB_HID_TYPE_BOOTPROTOCOL);
      }

      /* Get report packet length */
      usb_shhid_report_length = R_usb_hhid_GetReportLength();

      /* Set mouse mode data receive wait state */
      usb_hhid_smp_status_set(USB_HHID_APL_OFFSET_RX_WAIT);

      /* Initial LCD message */
      usb_hhid_smpl_get_report(usb_shhid_smpl_trn_data, usb_shhid_report_length);
    }
    break;

    /* Control transfer complete */
  case USB_HHID_APL_KBD_CTL_END:
    /* Set keyboard mode data receive wait state */
    usb_hhid_smp_status_set(USB_HHID_APL_OFFSET_RX_WAIT);
    break;

    /* Keyboard mode data receive wait state & Mouse mode data receive wait state */
  case USB_HHID_APL_KBD_RX_WAIT:
    /* continue */
  case USB_HHID_APL_MSE_RX_WAIT:
    /* Check suspend/resume switch (RSK SW3) */
    if (key_data == USB_HHID_SW3_ON)
    {
      R_usb_hhid_TransferEnd();   /* data transfer end */

      /* Set suspend state */
      usb_hhid_smp_status_set(USB_HHID_APL_OFFSET_SUSPEND);

      /* SUSPEND */
      R_usb_hhid_ChangeDeviceState(USB_DO_GLOBAL_SUSPEND, g_usb_hsmpl_devport, usb_hsmpl_device_state);
    }
    break;

    /* Keyboard Mode data receive complete State & Mouse Mode data receive complete State */
  case USB_HHID_APL_KBD_RX:
    /* continue */
  case USB_HHID_APL_MSE_RX:
    /* Host HID task status check */
    if (usb_shhid_apl_function == USB_HHID_APL_KBD_RX)
    {
      /* Keyboard data receive process */
      usb_hhid_smpl_kbd_data((uint8_t*)&usb_shhid_smpl_trn_data); /* Keyboard data receive process */
    }

    /* Suspend/Resume Sw(RSK Sw3) check */
    if (key_data == USB_HHID_SW3_ON)
    {
      /* Suspend State set */
      usb_hhid_smp_status_set(USB_HHID_APL_OFFSET_SUSPEND);
      /* SUSPEND */
      R_usb_hhid_ChangeDeviceState(USB_DO_GLOBAL_SUSPEND, g_usb_hsmpl_devport, usb_hsmpl_device_state);
    }
    else
    {
      /* data receive wait State set */
      usb_hhid_smp_status_set(USB_HHID_APL_OFFSET_RX_WAIT);
      usb_hhid_smpl_get_report(usb_shhid_smpl_trn_data, usb_shhid_report_length);
    }
    break;

    /* Keyboard Mode SUSPEND State & Mouse Mode SUSPEND State */
  case USB_HHID_APL_KBD_SUSPEND:
    /* continue */
  case USB_HHID_APL_MSE_SUSPEND:
    /* Suspend/Resume Sw(RSK Sw3) check */
    if (key_data == USB_HHID_SW3_ON)
    {
      /* RESUME */
      R_usb_hhid_ChangeDeviceState(USB_DO_GLOBAL_RESUME, g_usb_hsmpl_devport, usb_hsmpl_device_state);
    }
    break;

    /* Keyboard Mode RESUME State */
  case USB_HHID_APL_KBD_RESUME:
    /* Num Lock On */
    usb_hhid_smpl_kbd_led_ctl(USB_HHID_NUM_LOCK);
    SET_USB_SPCKEY(NUM_LOCK);
    /* Mouse Mode RESUME State */
  case USB_HHID_APL_MSE_RESUME:
    /* data receive wait State set */
    usb_hhid_smp_status_set(USB_HHID_APL_OFFSET_RX_WAIT);
    usb_hhid_smpl_get_report(usb_shhid_smpl_trn_data, usb_shhid_report_length);
    break;

  default:
    break;
  }
}

/******************************************************************************
Function Name   : usb_hapl_registration
Description     : Host Sample Driver Registration
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_hapl_registration(void)
{
  usb_hcdreg_t    driver;

  /* Driver registration */
  driver.ifclass          = USB_IFCLS_HID;            /* HID class */
  driver.classcheck       = &usb_hhid_class_check;
  driver.statediagram     = &usb_hsmpl_device_state;
  R_usb_hstd_DriverRegistration(&driver);
  R_usb_hhid_DriverStart();
}

/******************************************************************************
Function Name   : usb_hhid_class_check
Description     : Class Check
Arguments       : uint8_t **table         : Data Table Pointer
Return value    : none
******************************************************************************/
USB_STATIC void usb_hhid_class_check(uint8_t **table)
{
  R_usb_hhid_ClassCheck(table);
  usb_shhid_smpl_devaddr = (usb_addr_t)(*table[2]);
}

/******************************************************************************
Function Name   : usb_hsmpl_device_state
Description     : Open / Close
Arguments       : uint16_t data          : Device address
: uint16_t state            : Device state
Return value    : none
******************************************************************************/
USB_STATIC void usb_hsmpl_device_state(uint16_t data, uint16_t state)
{
  switch( state )
  {
  case USB_STS_DETACH:
    usb_smpl_set_suspend_flag(USB_NO);
    usb_shhid_active        = USB_NO;
    usb_shhid_apl_function  = USB_HHID_APL_CLOSE;  /* Set close state for demo sample application */
    R_usb_hhid_DriverStop();
    break;
  case USB_STS_ATTACH:
    usb_smpl_set_suspend_flag(USB_NO);
    g_usb_hsmpl_devport     = (usb_port_t)data;
    break;
  case USB_STS_DEFAULT:
    break;
  case USB_STS_CONFIGURED:
    if (data != 0)
    {
      usb_shhid_smpl_devaddr  = (usb_addr_t)data;         /* Device address store */
      R_usb_hhid_SetPipeRegistration((usb_addr_t)data);   /* Host HID Pipe registration */
      usb_shhid_active        = USB_YES;                  /* Host HID Driver Active */
      usb_shhid_apl_function  = USB_HHID_APL_INIT;        /* Set Init status for Host HID application. */
    }
    break;
  case USB_STS_SUSPEND:
    usb_smpl_set_suspend_flag(USB_YES);
    break;
  case USB_STS_RESUME:
    usb_smpl_set_suspend_flag(USB_NO);
    usb_hhid_smp_status_set(USB_HHID_APL_OFFSET_RESUME);
    break;
  case USB_STS_WAKEUP:
    usb_smpl_set_suspend_flag(USB_NO);
    usb_hhid_smp_status_set(USB_HHID_APL_OFFSET_RESUME);
    break;
  case USB_STS_POWER:
    break;
  case USB_STS_PORTOFF:
    break;
  case USB_STALL_SUCCESS:
    break;
  default:
    break;
  }

}

/******************************************************************************
Function Name   : usb_hhid_smpl_data_trans_result
Description     : data transfer complete process.
Arguments       : usb_utr_t *mess          : message
Return value    : none
******************************************************************************/
USB_STATIC void usb_hhid_smpl_data_trans_result(usb_utr_t *mess)
{
  /* Host HID task status check */
  if ((usb_shhid_apl_function == USB_HHID_APL_KBD_RX_WAIT) || (usb_shhid_apl_function == USB_HHID_APL_MSE_RX_WAIT))
  {
    /* Branch for Transfer Result */
    switch (mess->status)
    {
      /* data receive or transmission */
    case    USB_DATA_OK:
      /* continue */
    case    USB_DATA_SHT:
      /* Host HID Application State receive complete */
      usb_hhid_smp_status_set(USB_HHID_APL_OFFSET_RX);
      break;

      /* Control transfer stop(end) */
    case    USB_CTRL_END:
      /* Host HID Application State control transmission complete */
      usb_hhid_smp_status_set(USB_HHID_APL_OFFSET_CTL_END);
      break;

    case    USB_DATA_OVR:
      USB_DEBUG_HOOK(USB_DEBUG_HOOK_APL | USB_DEBUG_HOOK_CODE1);      /* error */
      break;

    case    USB_DATA_STALL:
      USB_DEBUG_HOOK(USB_DEBUG_HOOK_APL | USB_DEBUG_HOOK_CODE2);      /* error */
      break;

    case    USB_DATA_STOP:
      /* continue */
    case    USB_DATA_ERR:
      /* continue */
    case    USB_DATA_DTCH:
      break;

    default:
      USB_DEBUG_HOOK(USB_DEBUG_HOOK_APL | USB_DEBUG_HOOK_CODE3);      /* error */
      break;
    }
  }
}

/*============================================================================
      FUNCTION: USB_InsertCharInVector
      DESCRIPTION: Coloca o caracter pressionado dentro da mensagem de
      edição
      PARAMETERS:
              uint32_t key
                      Valor em caracter da tecla pressionada.
      RETURN: nada
============================================================================*/
void USB_InsertCharInVector (uint32_t key)
{
  static uint16_t Sys_AccentBufferAux;

  if(key == '´' && !Sys_AccentBuffer)
  {
    Sys_AccentBuffer = '´';
    Sys_AccentBufferAux = 1;
  }
  else if(key == '^' && !Sys_AccentBuffer)
  {
    Sys_AccentBuffer = '^';
    Sys_AccentBufferAux = 2;
  }
  else if(key == '~' && !Sys_AccentBuffer)
  {
    Sys_AccentBuffer = '~';
    Sys_AccentBufferAux = 3;
  }
  else if(key == '`' && !Sys_AccentBuffer)
  {
    Sys_AccentBuffer = '`';
    Sys_AccentBufferAux = 0;
  }
  else if(key == '¨' && !Sys_AccentBuffer)
  {
    Sys_AccentBuffer = '¨';
    Sys_AccentBufferAux = 4;
  }
  else if(key)
  {
    if(Sys_AccentBuffer)
    {
      uint16_t AuxOut = 0;
      if((Sys_AccentBufferAux >= 3) && (key != 'A') && (key != 'a') && (key != 'O') && (key != 'o'))
      {
        if(Sys_AccentBufferAux == 3)
        {
          AuxOut = 1;
        }
        else
        {
          Sys_AccentBufferAux--;
        }
      }
      if(!AuxOut)
      {
        switch(key)
        {
        case 'A':
          key = 'À'+Sys_AccentBufferAux;
          Sys_AccentBuffer = 0;
          break;
        case 'E':
          key = 'È'+Sys_AccentBufferAux;
          Sys_AccentBuffer = 0;
          break;
        case 'I':
          key = 'Ì'+Sys_AccentBufferAux;
          Sys_AccentBuffer = 0;
          break;
        case 'O':
          key = 'Ò'+Sys_AccentBufferAux;
          Sys_AccentBuffer = 0;
          break;
        case 'U':
          key = 'Ù'+Sys_AccentBufferAux;
          Sys_AccentBuffer = 0;
          break;
        case 'a':
          key = 'à'+Sys_AccentBufferAux;
          Sys_AccentBuffer = 0;
          break;
        case 'e':
          key = 'è'+Sys_AccentBufferAux;
          Sys_AccentBuffer = 0;
          break;
        case 'i':
          key = 'ì'+Sys_AccentBufferAux;
          Sys_AccentBuffer = 0;
          break;
        case 'o':
          key = 'ò'+Sys_AccentBufferAux;
          Sys_AccentBuffer = 0;
          break;
        case 'u':
          key = 'ù'+Sys_AccentBufferAux;
          Sys_AccentBuffer = 0;
          break;
        default:
          AuxOut = 1;
          break;
        }
      }
      if(AuxOut)
      {
        msg_edicao[Sys_EditPointMsg] = Sys_AccentBuffer;
        if(Sys_EditPointMsg < (MENSAGEM_MAX_TAMANHO-1))
        {
          Sys_EditPointMsg++;
        }
        if(msg_edicao[Sys_EditPointMsg] == 0)
        {
          msg_edicao[Sys_EditPointMsg] = ' ';
        }
        Sys_AccentBuffer = 0;
      }
    }
    msg_edicao[Sys_EditPointMsg] = key;
    if(Sys_EditPointMsg < (MENSAGEM_MAX_TAMANHO-1) && Sys_TypeOfMsgOnDisplay != SetFatorK)
    {
      Sys_EditPointMsg++;
    }
    //RETIRADO MARCOS
    //if(msg_edicao[Sys_EditPointMsg] == 0)
   // {
      //msg_edicao[Sys_EditPointMsg] = ' ';
    //}
  }
  return;
}

/***************************************************************************************************
Function Name   : usb_hhid_smpl_kbd_data
Description     : Keyboard data receive process.
Arguments       : uint8_t *data             :USB Keyboard Data
Return value    : none
***************************************************************************************************/
USB_STATIC void usb_hhid_smpl_kbd_data(uint8_t *data) //QUANDO RECEBE ALGUMA TECLA
{
  static uint16_t Sys_FunctionDelay;
  extern uint8_t EstadoTeste;
  /* Keyboard input data check */
  if (data[USB_HHID_KBD_MODIFIERKEY_INDEX] != USB_HHID_KBD_NOT_PUSH)
  {
    usb_shhid_ModifierKeys    = data[USB_HHID_KBD_MODIFIERKEY_INDEX];
  }
  else
  {
    usb_shhid_ModifierKeys    = USB_HHID_KBD_NOT_PUSH;
  }
  /* Input Keys */
  if ((data[USB_HHID_KBD_KEYCODE1_INDEX] != USB_HHID_KBD_NOT_PUSH) && (usb_shhid_KbdKey != data[USB_HHID_KBD_KEYCODE1_INDEX]) && Sys_TypeOfMsgOnDisplay != OtherMessage)
  {
    usb_shhid_KbdKey    = data[USB_HHID_KBD_KEYCODE1_INDEX];

    if (usb_shhid_KbdKey == 0x17 && usb_shhid_ModifierKeys == 0x03)
      Sys_TypeOfMsgOnDisplay = ModoTeste;//
    if (Sys_TypeOfMsgOnDisplay == ModoTeste)
    {
      if (usb_shhid_KbdKey == 0x1e)//numero 1
          EstadoTeste = 0;
      if (usb_shhid_KbdKey == 0x1f)//numero 2
          EstadoTeste = 1;
      if (usb_shhid_KbdKey == 0x20)//numero 3
          EstadoTeste = 2;
      if (usb_shhid_KbdKey == 0x21)//numero 4
          EstadoTeste = 3;
      if (usb_shhid_KbdKey == 0x22)//numero 5
          EstadoTeste = 4;
      if (usb_shhid_KbdKey == 0x23)//numero 6
          EstadoTeste = 5;
      if (usb_shhid_KbdKey == 0x24)//numero 7
          EstadoTeste = 6;
      if (usb_shhid_KbdKey == 0x25)//numero 8
          EstadoTeste = 7;
      if (usb_shhid_KbdKey == 0x26)//numero 9
          EstadoTeste = 8;
      if (usb_shhid_KbdKey == 0x27)
          EstadoTeste = 9;
      if (usb_shhid_KbdKey == 0x16)//SIM
          EstadoTeste = 11;
      if (usb_shhid_KbdKey == 0x11)//NAO
          EstadoTeste = 12;
    }

    else if((Sys_TypeOfMsgOnDisplay == EditMessage))
    {
      /* 'a' <= usb_shhid_KbdKey <= 'z' */
      if((usb_shhid_KbdKey >= 0x04) && ((usb_shhid_KbdKey <= 0x1D)))
      {
        if(RD_USB_SPCKEY(CAPS_LOCK) )
        {
          if(RD_USB_MODIFKEY(RIGHT_SHIFT) || RD_USB_MODIFKEY(LEFT_SHIFT))
          {
            USB_InsertCharInVector(ScanCodes[usb_shhid_KbdKey-0x04][0]);
          }
          else
          {

            USB_InsertCharInVector(ScanCodes[usb_shhid_KbdKey-0x04][1]);
          }
        }
        else
        {
          if(RD_USB_MODIFKEY(RIGHT_SHIFT) || RD_USB_MODIFKEY(LEFT_SHIFT))
          {
            USB_InsertCharInVector(ScanCodes[usb_shhid_KbdKey-0x04][1]);
          }
          else
          {

            USB_InsertCharInVector(ScanCodes[usb_shhid_KbdKey-0x04][0]);
          }
        }
      }
      /* '1' <= usb_shhid_KbdKey <= '0' */
      else if((usb_shhid_KbdKey >= 0x1E) && ((usb_shhid_KbdKey <= 0x27)))
      {
        if(RD_USB_MODIFKEY(RIGHT_SHIFT) || RD_USB_MODIFKEY(LEFT_SHIFT))
        {
          USB_InsertCharInVector(ScanCodes[usb_shhid_KbdKey-0x04][1]);
        }
        else
        {
          USB_InsertCharInVector(ScanCodes[usb_shhid_KbdKey-0x04][0]);
        }
      }
      /* ' ' <= usb_shhid_KbdKey <= '/' */
      else if((usb_shhid_KbdKey >= 0x2C) && ((usb_shhid_KbdKey <= 0x38)))
      {
        if(RD_USB_MODIFKEY(RIGHT_SHIFT) || RD_USB_MODIFKEY(LEFT_SHIFT))
        {
          USB_InsertCharInVector(ScanCodes[usb_shhid_KbdKey-0x08][1]);
        }
        else
        {
          USB_InsertCharInVector(ScanCodes[usb_shhid_KbdKey-0x08][0]);
        }
      }
      /* '/' <= usb_shhid_KbdKey <= '+' */
      else if((usb_shhid_KbdKey >= 0x54) && ((usb_shhid_KbdKey <= 0x57)))
      {
        if(RD_USB_MODIFKEY(RIGHT_SHIFT) || RD_USB_MODIFKEY(LEFT_SHIFT))
        {
          USB_InsertCharInVector(ScanCodes[usb_shhid_KbdKey-0x23][1]);
        }
        else
        {
          USB_InsertCharInVector(ScanCodes[usb_shhid_KbdKey-0x23][0]);
        }
      }
      /* '1' <= usb_shhid_KbdKey <= '0' */
      else if((usb_shhid_KbdKey >= 0x59) && ((usb_shhid_KbdKey <= 0x64)))
      {
        //if(RD_USB_SPCKEY(NUM_LOCK))
        {
          if(RD_USB_MODIFKEY(RIGHT_SHIFT) || RD_USB_MODIFKEY(LEFT_SHIFT))
          {
            USB_InsertCharInVector(ScanCodes[usb_shhid_KbdKey-0x24][1]);
          }
          else
          {
            USB_InsertCharInVector(ScanCodes[usb_shhid_KbdKey-0x24][0]);
          }
        }
      }
      /* usb_shhid_KbdKey == '=' */
      else if(usb_shhid_KbdKey >= 0x67)
      {
        if(RD_USB_MODIFKEY(RIGHT_SHIFT) || RD_USB_MODIFKEY(LEFT_SHIFT))
        {
          USB_InsertCharInVector(ScanCodes[65][1]);
        }
        else
        {
          USB_InsertCharInVector(ScanCodes[65][0]);
        }
      }


      /* Keyboard input data check Enter */
      //FINALIZA A PROGRAMAÇÃO DA MSG
      else if ((usb_shhid_KbdKey == 0x28) || (usb_shhid_KbdKey == 0x58))       /* == 0x28 || 0x58 */
      {
        /* ASCII CODE SET '\n' */
        /* Caso esteja no modo normal de mensagem, aceita o enter */
        if(Sys_TypeOfMsgOnDisplay == EditMessage)
        {
          uint32_t i;
          for(i = 0;i<MENSAGEM_MAX_TAMANHO;i++)
          {
            //TRANSFERE O TEMPORARIO PRO DEFINITIVO
            if (msg_edicao[i]!=0)
              flash.msg[Sys_FunctionSelected-1][i] = msg_edicao[i];
            else flash.msg[Sys_FunctionSelected-1][i] = 0;
            msg_edicao[i]=0;
          }
          for(;i<MENSAGEM_MAX_TAMANHO;i++)
          {
            flash.msg[Sys_FunctionSelected-1][i] = 0;//COMPLETA A MSG COM ZEROS
            msg_edicao[i]=0;
          }

          Sys_TypeOfMsgOnDisplay = NormalMessage; /* TODO - ALISON */
          //if(Sys_FunctionSelected == 1)
         // {

          //apenas para começar d novo
          Sys_MessagePointer = 0xFF;

          ZeraEfeito();
          FDL_WriteControl();
         // }
         // else
         // {
         //   Sys_MessagePointer = Sys_FunctionSelected - 1;
         // }
          //Sys_DisplayFlags |= (NeedToChangeActualMessage | NeedToRefreshNow);
        }
      }
      /* Keyboard input data check ESC */
      else if (usb_shhid_KbdKey == USB_HHID_KBD_CODE_ESC)       /* == 0x29 */
      {
        /* ASCII CODE SET ESC */
        /* Caso esteja no modo normal de mensagem, aceita o esc */
        if(Sys_TypeOfMsgOnDisplay == EditMessage)
        {
          Sys_TypeOfMsgOnDisplay = NormalMessage;
		  Sys_MessagePointer = Sys_GetMessagePointerNext();
          ZeraEfeito();
        }
      }
      /* Keyboard input data check BackSpace */
      else if (usb_shhid_KbdKey == USB_HHID_KBD_CODE_BS)          /* == 0x2A */
      {
        if(Sys_EditPointMsg)
        {
          uint32_t i = 0;
          for( ;(i + Sys_EditPointMsg) < MENSAGEM_MAX_TAMANHO;i++)
          {
            //FAZ O DESLOCAMENTO DO BUFFER PARA  ESQUERDA
            msg_edicao[i + Sys_EditPointMsg - 1] = msg_edicao[i + Sys_EditPointMsg];
          }
          msg_edicao[i + Sys_EditPointMsg - 1] = 0;
          Sys_EditPointMsg--;
        }
      }
      /* Keyboard input delete */
      else if ((usb_shhid_KbdKey == USB_HHID_KBD_CODE_DEL)) /* == 0x4C */
      {
        if((Sys_EditPointMsg < MENSAGEM_MAX_TAMANHO) && msg_edicao[Sys_EditPointMsg + 1])
        {
          uint32_t i = 0;
          for(;i<MENSAGEM_MAX_TAMANHO-1;i++)
          {
            msg_edicao[i + Sys_EditPointMsg] = msg_edicao[i + Sys_EditPointMsg +1];
          }
          msg_edicao[i + Sys_EditPointMsg] = 0;
        }
        else
        {
            msg_edicao[Sys_EditPointMsg] = ' ';
        }
      }
      /* Keyboard Direcional */
      else if ((usb_shhid_KbdKey <= USB_HHID_KBD_CODE_DIRUP))       /* 0x53 */
      {
        /* Verifica tecla */
        switch(usb_shhid_KbdKey)
        {
        case 0x4F: /* Dir. Direita */
          //VERIFICA SE TEM ALGO ESCRITO, E SE AINDA NÃO ATINGIU O TAMANHO MÁXIMO
          if(msg_edicao[Sys_EditPointMsg + 1] && (Sys_EditPointMsg < (MENSAGEM_MAX_TAMANHO-1)))
          {
            Sys_EditPointMsg++;
          }
          break;
        case 0x50: /* Dir. Esquerda */
          if(Sys_EditPointMsg)
          {//VERIFICA SE NÃO ATINGIOU O TAMANHO MÍNIMO
            Sys_EditPointMsg--;
          }
          break;
        case 0x51: /* Dir. Baixo */
        case 0x52: /* Dir. Cima */

          break;
        }
      }
    }
    else
    {
      /* Keyboard input function check '1' to '12' */
      if ((usb_shhid_KbdKey >= 0x3A) && (usb_shhid_KbdKey <= 0x45))            /* <= Function 12 (0x45) */
      {
        /* Function set F1 to F12*/
        usb_shhid_FunctionKey = usb_shhid_KbdKey - USB_HHID_KEYCODE_TO_ASCII_FOFFSET;
        (void)Delay_mS(&Sys_FunctionDelay,1000);//UM SEGUNDO PRESSIONANDO PARA PODER EDITAR
      }
      //else if((usb_shhid_KbdKey >= 0x1E) && ((usb_shhid_KbdKey <= 0x27)))
      else
      {
        USB_InsertCharInVector(ScanCodes[usb_shhid_KbdKey-0x04][1]);

        //IMPLEMENTAR O CONTADOR AQUI, PARA VER EM QUAL DOS VETORES TEM QUE SALVAR... E SALVAR DEPOIS DISSO

        if (usb_shhid_KbdKey == 0x24 || usb_shhid_KbdKey == 0x5f)//numero 7
        {
          ZeraEfeito();
          Sys_TypeOfMsgOnDisplay = OtherMessage;
          if ((flash.msg[11][249])&0x01)
          {
            flash.msg[11][249] &= 0xFE;
            //Sys_DisplayFlags = 0;
            //Sys_DisplayFlags |= NeedToChangeActualMessage;
            msg_edicao[0] = 'h'; //MOSTRA A VERSÃO DO FIRMWARE
            msg_edicao[1] = '-';
            msg_edicao[2] = 'o';
            msg_edicao[3] = 'f';
            msg_edicao[4] = 'f';
          }
          else
          {
            flash.msg[11][249] |= 0x01;
            msg_edicao[0] = 'h'; //MOSTRA A VERSÃO DO FIRMWARE
            msg_edicao[1] = '-';
            msg_edicao[2] = 'o';
            msg_edicao[3] = 'n';
          }
          FDL_WriteControl();
          //ZeraEfeito();

        }

        else if (usb_shhid_KbdKey == 0x26 || usb_shhid_KbdKey == 0x61)//numero 9
        {
          ZeraEfeito();
          Sys_TypeOfMsgOnDisplay = OtherMessage;
          if ((flash.msg[11][249])&0x04)
          {
            flash.msg[11][249] &= 0xFB;
            msg_edicao[0] = 'i';
            msg_edicao[1] = '-';
            msg_edicao[2] = 'o';
            msg_edicao[3] = 'f';
            msg_edicao[4] = 'f';
          }
          else
          {
            flash.msg[11][249] |= 0x04;
            msg_edicao[0] = 'i';
            msg_edicao[1] = '-';
            msg_edicao[2] = 'o';
            msg_edicao[3] = 'n';
          }
          FDL_WriteControl();
        }
        else if (usb_shhid_KbdKey == 0x27 || usb_shhid_KbdKey == 0x62)//numero 0
        {
          ZeraEfeito();
          Sys_TypeOfMsgOnDisplay = OtherMessage;
          if ((flash.msg[11][249])&0x08)
          {
            flash.msg[11][249] &= 0xF7;
            msg_edicao[0] = 'e';
            msg_edicao[1] = '-';
            msg_edicao[2] = 'o';
            msg_edicao[3] = 'f';
            msg_edicao[4] = 'f';
          }
          else
          {
            flash.msg[11][249] |= 0x08;
            msg_edicao[0] = 'e';
            msg_edicao[1] = '-';
            msg_edicao[2] = 'o';
            msg_edicao[3] = 'n';
          }
          FDL_WriteControl();
        }
		else if (usb_shhid_KbdKey == 0x25 || usb_shhid_KbdKey == 0x60) //numero 8
        {
          ZeraEfeito();
          Sys_TypeOfMsgOnDisplay = OtherMessage;

          if ((flash.msg[11][249])&0x02)
          {
            flash.msg[11][249] &= 0xFD;
            msg_edicao[0] = 't';
            msg_edicao[1] = '-';
            msg_edicao[2] = 'o';
            msg_edicao[3] = 'f';
            msg_edicao[4] = 'f';
          }
          else
          {
            flash.msg[11][249] |= 0x02;
            msg_edicao[0] = 't';
            msg_edicao[1] = '-';
            msg_edicao[2] = 'o';
            msg_edicao[3] = 'n';
          }
          FDL_WriteControl();
        }
        else if (usb_shhid_KbdKey == 0x52 && Sys_TypeOfMsgOnDisplay != SetFatorK)//pra cima
        {
          flash.msg[0][245] = 0;
          flash.msg[1][245] = 0;
          flash.msg[2][245] = 0;
          flash.msg[3][245] = 0;
          flash.msg[4][245] = 0;
          flash.msg[5][245] = 0;
          flash.msg[6][245] = 0;
          flash.msg[7][245] = 0;
          flash.msg[8][245] = 0;
          flash.msg[9][245] = 0;
          flash.msg[10][245] = 0;
          flash.msg[11][245] = 0;
          FDL_WriteControl();
          ZeraEfeito();
        }
        else if (usb_shhid_KbdKey == 0x51 && Sys_TypeOfMsgOnDisplay != SetFatorK)//pra baixo
        {
          flash.msg[0][245] = 1;
          flash.msg[1][245] = 1;
          flash.msg[2][245] = 1;
          flash.msg[3][245] = 1;
          flash.msg[4][245] = 1;
          flash.msg[5][245] = 1;
          flash.msg[6][245] = 1;
          flash.msg[7][245] = 1;
          flash.msg[8][245] = 1;
          flash.msg[9][245] = 1;
          flash.msg[10][245] = 1;
          flash.msg[11][245] = 1;
          FDL_WriteControl();
          ZeraEfeito();
        }
        else if (usb_shhid_KbdKey == 0x4F && Sys_TypeOfMsgOnDisplay != SetFatorK)//pra direita
        {
          flash.msg[0][245] = 2;
          flash.msg[1][245] = 2;
          flash.msg[2][245] = 2;
          flash.msg[3][245] = 2;
          flash.msg[4][245] = 2;
          flash.msg[5][245] = 2;
          flash.msg[6][245] = 2;
          flash.msg[7][245] = 2;
          flash.msg[8][245] = 2;
          flash.msg[9][245] = 2;
          flash.msg[10][245] = 2;
          flash.msg[11][245] = 2;
          FDL_WriteControl();
          ZeraEfeito();
        }
        else if (usb_shhid_KbdKey == 0x50 && Sys_TypeOfMsgOnDisplay != SetFatorK)//pra esquerda
        {
          flash.msg[0][245] = 3;
          flash.msg[1][245] = 3;
          flash.msg[2][245] = 3;
          flash.msg[3][245] = 3;
          flash.msg[4][245] = 3;
          flash.msg[5][245] = 3;
          flash.msg[6][245] = 3;
          flash.msg[7][245] = 3;
          flash.msg[8][245] = 3;
          flash.msg[9][245] = 3;
          flash.msg[10][245] = 3;
          flash.msg[11][245] = 3;
          FDL_WriteControl();
          ZeraEfeito();
        }
        if (usb_shhid_KbdKey == 0x19)
        {

          Sys_TypeOfMsgOnDisplay = SetFatorK;
          Sys_TachC = FatorK/100;
          Sys_TachD = (FatorK-Sys_TachC*100)/10;
          Sys_TachU = FatorK-Sys_TachC*100 - Sys_TachD*10;
          for (uint8_t i=0;i<240;i++)
            msg_edicao[i]=0;
          Sys_DisplayFlags |= NeedToFlowWithNothing;
          Sys_EditPointMsg = 2;
        }
        if (Sys_TypeOfMsgOnDisplay == SetFatorK)
        {
          if ((usb_shhid_KbdKey == 0x28) || (usb_shhid_KbdKey == 0x58))
          {
            extern unsigned int FatorK;
            extern unsigned char Sys_TachU, Sys_TachD, Sys_TachC;
            flash.msg[0][251] = FatorK/256; //salva a parte alta
            flash.msg[0][252] = FatorK%256; //salva a parte baixa
            FDL_WriteControl();
            Sys_TypeOfMsgOnDisplay = NormalMessage;
            ZeraEfeito();
          }
          if (usb_shhid_KbdKey == 0x50)//esquerda
          {
            if(Sys_EditPointMsg)
              Sys_EditPointMsg--;
          }
          if (usb_shhid_KbdKey == 0x4f)//direita
          {
             if(Sys_EditPointMsg<2)
               Sys_EditPointMsg++;
          }
          if (usb_shhid_KbdKey == 0x52)//cima
          {
            if (Sys_EditPointMsg == 2 && Sys_TachU<9)
              Sys_TachU++;
            if (Sys_EditPointMsg == 1 && Sys_TachD<9)
              Sys_TachD++;
            if (Sys_EditPointMsg == 0 && Sys_TachC<9)
              Sys_TachC++;
          }
          if (usb_shhid_KbdKey == 0x51)//baixo
          {
            if (Sys_EditPointMsg == 2 && Sys_TachU>0)
              Sys_TachU--;
            if (Sys_EditPointMsg == 1 && Sys_TachD>0)
              Sys_TachD--;
            if (Sys_EditPointMsg == 0 && Sys_TachC>0)
              Sys_TachC--;
          }
        }

        if (Sys_ActualDisplayMessage<12 && Sys_TypeOfMsgOnDisplay != OtherMessage)
        {
          if (usb_shhid_KbdKey>=0x04&&usb_shhid_KbdKey<=0x14)
          {
            //rotinas para zerar a mensagens quando troca efeito ou outras configs
            ZeraEfeito();
            trocou_parametro = 1;
            //
            flash.msg[Sys_ActualDisplayMessage][240] = usb_shhid_KbdKey-0x04;
            FDL_WriteControl();
            //EFEITO
          }
          if (usb_shhid_KbdKey == 0x1e || usb_shhid_KbdKey == 0x59)//numero 1
          {//usa o cnt das msg para escolher...
            flash.msg[Sys_ActualDisplayMessage][242] = 7;
            ZeraEfeito();
            trocou_parametro = 1;
            //
            FDL_WriteControl();
          }
          else if (usb_shhid_KbdKey == 0x1f || usb_shhid_KbdKey == 0x5a)//numero 2
          {
            flash.msg[Sys_ActualDisplayMessage][242] = 11;
            ZeraEfeito();
            trocou_parametro = 1;
            //
            FDL_WriteControl();
          }
          else if (usb_shhid_KbdKey == 0x20 || usb_shhid_KbdKey == 0x5b)//numero 3
          {
            flash.msg[Sys_ActualDisplayMessage][242] = 19;
            ZeraEfeito();
            trocou_parametro = 1;
            //
            FDL_WriteControl();
          }
          else if (usb_shhid_KbdKey == 0x21 || usb_shhid_KbdKey == 0x5c)//numero 4
          {
              flash.msg[Sys_ActualDisplayMessage][244] = 80;
              //rotinas para zerar a mensagens quando troca efeito ou outras configs
            ZeraEfeito();
            trocou_parametro = 1;
            //
            FDL_WriteControl();
          }
          else if (usb_shhid_KbdKey == 0x22 || usb_shhid_KbdKey == 0x5d)//numero 5
          {
              flash.msg[Sys_ActualDisplayMessage][244] = 170;
              //rotinas para zerar a mensagens quando troca efeito ou outras configs
            ZeraEfeito();
            trocou_parametro = 1;
            //
            FDL_WriteControl();
          }
          else if (usb_shhid_KbdKey == 0x23 || usb_shhid_KbdKey == 0x5e)//numero 6
          {
              flash.msg[Sys_ActualDisplayMessage][244] = 250;
              //rotinas para zerar a mensagens quando troca efeito ou outras configs
            ZeraEfeito();
            trocou_parametro = 1;
            //
            FDL_WriteControl();
          }
        }

      }
    }
    /* Keyboard input data check Num Lock */
    if(usb_shhid_KbdKey == USB_HHID_KBD_CODE_NUMLOCK) /* == Num lock */
    {
      /* Controle de Num Lock */
      if(RD_USB_SPCKEY(NUM_LOCK))
      {
        CLR_USB_SPCKEY(NUM_LOCK);
      }
      else
      {
        SET_USB_SPCKEY(NUM_LOCK);
      }
      usb_hhid_smpl_kbd_led_ctl(usb_shhid_SpecialKeys);
    }
    /* Keyboard input data check Caps Lock */
    else if(usb_shhid_KbdKey == USB_HHID_KBD_CODE_CAPSLOCK) /* == Caps lock */
    {
      /* Controle de Num Lock */
      if(RD_USB_SPCKEY(CAPS_LOCK))
      {
        CLR_USB_SPCKEY(CAPS_LOCK);

      }
      else
      {
        SET_USB_SPCKEY(CAPS_LOCK);
      }
      usb_hhid_smpl_kbd_led_ctl(usb_shhid_SpecialKeys);
    }
    /* Keyboard input data check Scroll Lock */
    else if(usb_shhid_KbdKey == USB_HHID_KBD_CODE_SCROLLLOCK) /* == Scroll lock */
    {
      /* Controle de Scroll Lock */
      if(RD_USB_SPCKEY(SCROLL_LOCK))
      {
        CLR_USB_SPCKEY(SCROLL_LOCK);
      }
      else
      {
        SET_USB_SPCKEY(SCROLL_LOCK);
      }
      usb_hhid_smpl_kbd_led_ctl(usb_shhid_SpecialKeys);
    }
  }
  else
  {
    usb_shhid_KbdKey    = 0;

    /* Se a pressionada anterior foi de um FX */
    if(usb_shhid_FunctionKey != 0)
    {
      if(usb_shhid_ModifierKeys==0x01)//(Delay_mS(&Sys_FunctionDelay,1000))
      {//AQUI ENTRA NO MODO DE EDIÇÃO
        uint32_t i = 0;
        ZeraEfeito();
        Sys_TypeOfMsgOnDisplay = EditMessage; /* TODO - ALISON */
        Sys_FunctionSelected = usb_shhid_FunctionKey;
        /* Iguala a mensagem a ser editada a mensagem 17 (mensagem de edição) */
        for (uint8_t i=0;i<MENSAGEM_MAX_TAMANHO;i++)
          msg_edicao[i]=0;
        for(;flash.msg[Sys_FunctionSelected-1][i] && i != MENSAGEM_MAX_TAMANHO;i++)
        {
          msg_edicao[i] = flash.msg[Sys_FunctionSelected-1][i];//matriz temporaria;
          //a segunda matriz vira uma temporária (matriz para edição)
          //ZERAR A MATRIZ TEMPORARIA
        }
        Sys_EditPointMsg = 0;
        Sys_ConvertASCIItoMatriz(0);
        Sys_DisplayFlags |= NeedToFlowWithNothing;
      }
      else
      {
        for (uint8_t i=0;i<240;i++)
          msg_edicao[i]=0;
        //AQUI HABILITA OU DESABILITA AS MSGS
        Sys_FunctionDelay = 0;
        /* Teste o status da mensagem */
        Sys_FunctionSelected = usb_shhid_FunctionKey;

        Sys_DisplayFlags &= ~NeedToNormalFlow;
        Sys_DisplayFlags &= ~NeedToStopFlow;
        Sys_DisplayFlags &= ~NeedToFlowWithNothing;
        Sys_DisplayFlags |= NeedToChangeActualMessage;
        //LimpaTela();
        ZeraEfeito();
        Sys_TypeOfMsgOnDisplay = OtherMessage;

       // uint32_t i = 7;
        //matriz temporaria pra receber essa encrenca
        msg_edicao[0] = 'm';
        msg_edicao[1] = '-';
        msg_edicao[2] = 'o';
        //SE ESTIVER HABILITADA
        flash.msg[usb_shhid_FunctionKey-1][241] = !flash.msg[usb_shhid_FunctionKey-1][241];

        if (flash.msg[usb_shhid_FunctionKey-1][241])
        {
          msg_edicao[3] = 'n';
          msg_edicao[4] = 0;
          trocou_parametro = 1;//VAI PARA A MENSAGEM QUE É ACIONADA
          Sys_MessagePointer = (usb_shhid_FunctionKey-1);
          Sys_ActualDisplayMessage = Sys_MessagePointer;
        }
        else
        {
          msg_edicao[3] = 'f';
          msg_edicao[4] = 'f';
        }
        FDL_WriteControl();
      }
      usb_shhid_FunctionKey = 0;
    }
  }
}

/******************************************************************************
Function Name   : usb_hhid_smp_status_set
Description     : Host HID Sample Application Status Update
Arguments       : uint16_t status   : Set Status code
Return value    : none
******************************************************************************/
USB_STATIC void usb_hhid_smp_status_set(uint16_t status)
{
  usb_shhid_apl_function &= USB_HHID_APL_MODE_MASK;
  usb_shhid_apl_function |= status;
}

/******************************************************************************
Function Name   : usb_hhid_smpl_kbd_led_ctl
Description     : USB Keyboard LED control (NumLock, CapsLock, ScrollLock)
Arguments       : uint16_t mode  : Set light keyboard LED.(NUM_LOCK/CAPS_LOCK/SCROLL_LOCK)
Return value    : none
******************************************************************************/
USB_STATIC void usb_hhid_smpl_kbd_led_ctl(uint16_t mode)
{
  usb_shhid_set_report_data[0] = (uint8_t)mode;

  /* Host HID class request SetReport */
  usb_hhid_smpl_set_report(usb_shhid_set_report_data, 1);
}

/******************************************************************************
Function Name   : usb_hhid_smpl_set_report
Description     : Host HID class request SetReport
Arguments       : uint16_t  *p_data       : Command data top address
: uint16_t  length         : Command length
Return value    : none
******************************************************************************/
USB_STATIC void usb_hhid_smpl_set_report(uint8_t *p_data, uint16_t length)
{
  /* Set class request setup packet bRequest */
  g_usb_hsmpl_request.bRequestCode = USB_HID_SET_REPORT;

  g_usb_hsmpl_request.tranadr     = p_data;                               /* Command data top address */
  g_usb_hsmpl_request.tranlen     = length;                               /* Command length */
  g_usb_hsmpl_request.devadr      = usb_shhid_smpl_devaddr;               /* Device address */
  g_usb_hsmpl_request.complete    = (usb_cb_t)&usb_hsmpl_class_result;    /* Set callback function */

  /* Issue host HID class request */
  R_usb_hhid_class_request(&g_usb_hsmpl_request);
}

/******************************************************************************
Function Name   : usb_hhid_smpl_get_report
Description     : Host HID class request GetReport
Arguments       : uint16_t  *p_data       : Report storage address
: uint16_t  length         : Report receive size
Return value    : none
******************************************************************************/
USB_STATIC void usb_hhid_smpl_get_report(uint8_t *p_data, uint16_t length)
{
#ifdef USB_HHID_GET_REPORT_PIPE0
  /* Class request setup packet bRequest Set */
  g_usb_hsmpl_request.bRequestCode = USB_HID_GET_REPORT;

  g_usb_hsmpl_request.tranadr     = p_data;                               /* Report storage address */
  g_usb_hsmpl_request.tranlen     = length;                               /* Report receive size */
  g_usb_hsmpl_request.devadr      = usb_shhid_smpl_devaddr;               /* Device address */
  g_usb_hsmpl_request.complete    = (usb_cb_t)&usb_hsmpl_class_result;    /* Set callback function */

  /* Issue host HID class request */
  R_usb_hhid_class_request(&g_usb_hsmpl_request);
#else   /* USB_HHID_GET_REPORT_PIPE0 is not defined */
  /* Interrupt IN transfer request for Peripheral HID Device */
  R_usb_hhid_PipeTransferExample(p_data, length,(usb_cb_t)&usb_hhid_smpl_data_trans_result);
#endif  /* USB_HHID_GET_REPORT_PIPE0 */
}

/******************************************************************************
Function Name   : usb_hhid_smpl_set_protocol
Description     : Host HID class request SetReport
Arguments       : uint8_t  protocol      : Protocol(0:Boot,1:Report)
Return value    : none
******************************************************************************/
USB_STATIC void usb_hhid_smpl_set_protocol(uint8_t protocol)
{
  /* Class request setup packet bRequest Set */
  g_usb_hsmpl_request.bRequestCode = USB_HID_SET_PROTOCOL;

  g_usb_hsmpl_request.set_protocol    = protocol;                             /* Protocol(0:Boot,1:Report) */
  g_usb_hsmpl_request.tranadr         = USB_NULL;                             /* No-data control */
  g_usb_hsmpl_request.devadr          = usb_shhid_smpl_devaddr;               /* Device address */
  g_usb_hsmpl_request.complete        = (usb_cb_t)&usb_hsmpl_class_result;    /* Set callback function */

  /* Issue host HID class request */
  R_usb_hhid_class_request(&g_usb_hsmpl_request);
}

/******************************************************************************
Function Name   : usb_hsmpl_class_result
Description     : Host HID class request callback function
Arguments       : usb_utr_t *mess   : Message
Return value    : none
******************************************************************************/
USB_STATIC void usb_hsmpl_class_result(usb_utr_t *mess)
{
  if (mess->status == USB_CTRL_END)
  {
    switch(g_usb_hsmpl_request.bRequestCode)
    {
    case    USB_HID_GET_REPORT:
      usb_hhid_smpl_get_report_result();
      break;
    case    USB_HID_GET_HID_DESCRIPTOR:
      break;
    case    USB_HID_GET_REPORT_DESCRIPTOR:
      break;
    case    USB_HID_GET_PHYSICAL_DESCRIPTOR:
      break;
    case    USB_HID_SET_REPORT:
      usb_hhid_smpl_kbd_led_ctl_result();
      break;
    case    USB_HID_SET_IDLE:
      break;
    case    USB_HID_GET_IDLE:
      break;
    case    USB_HID_SET_PROTOCOL:
      break;
    case    USB_HID_GET_PROTOCOL:
      break;
    default:
      break;
    }
  }
  else
  {
  }
}

/******************************************************************************
Function Name   : usb_hhid_smpl_get_report_result
Description     : Debug Callbask(Get_Report Request)
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_hhid_smpl_get_report_result(void)
{
#ifdef USB_HHID_GET_REPORT_PIPE0
  /* Host HID Application State receive complete */
  usb_hhid_smp_status_set(USB_HHID_APL_OFFSET_RX);
#endif /* USB_HHID_GET_REPORT_PIPE0 */
}

/******************************************************************************
Function Name   : usb_hhid_smpl_kbd_led_ctl_result
Description     : CallBack Function
Arguments       : none
Return value    : none
******************************************************************************/
USB_STATIC void usb_hhid_smpl_kbd_led_ctl_result(void)
{

}

#ifdef  CLASS_REQUEST_SAMPLE
/******************************************************************************
Function Name   : usb_hhid_smpl_get_hid_descriptor
Description     : GET HID Descriptor
Arguments       : uint16_t  *p_data  : Descriptor data storage address
Return value    : none
******************************************************************************/
USB_STATIC void usb_hhid_smpl_get_hid_descriptor(uint8_t *p_data)
{
  /* Set class request setup packet bRequest */
  g_usb_hsmpl_request.bRequestCode = USB_HID_GET_HID_DESCRIPTOR;

  g_usb_hsmpl_request.tranadr     = p_data;                               /* HID Descriptor storage address */
  g_usb_hsmpl_request.devadr      = usb_shhid_smpl_devaddr;               /* Device address */
  g_usb_hsmpl_request.complete    = (usb_cb_t)&usb_hsmpl_class_result;    /* Set callback function */

  /* Issue host HID class request */
  R_usb_hhid_class_request(&g_usb_hsmpl_request);
}

/******************************************************************************
Function Name   : usb_hhid_smpl_get_report_descriptor
Description     : GET HID Report Descriptor
Arguments       : uint16_t  *p_data  : Descriptor data storage address
: uint16_t  length    : Descriptor length
Return value    : none
******************************************************************************/
USB_STATIC void usb_hhid_smpl_get_report_descriptor(uint8_t *p_data, uint16_t length)
{
  /*  Set class request setup packet bRequest */
  g_usb_hsmpl_request.bRequestCode = USB_HID_GET_REPORT_DESCRIPTOR;

  g_usb_hsmpl_request.tranadr     = p_data;                               /* Report descriptor storage address */
  g_usb_hsmpl_request.tranlen     = length;                               /* Report descriptor length */
  g_usb_hsmpl_request.devadr      = usb_shhid_smpl_devaddr;               /* Device address */
  g_usb_hsmpl_request.complete    = (usb_cb_t)&usb_hsmpl_class_result;    /* Set callback function */

  /* Issue host HID class request */
  R_usb_hhid_class_request(&g_usb_hsmpl_request);
}

/******************************************************************************
Function Name   : usb_hhid_smpl_get_physical_descriptor
Description     : Get HID physical descriptor.
Arguments       : uint16_t  *p_data      : Descriptor data storage address
: uint16_t length          : Descriptor length
: uint8_t  index            :
Return value    : none
******************************************************************************/
USB_STATIC void usb_hhid_smpl_get_physical_descriptor(uint8_t *p_data, uint16_t length, uint8_t index)
{
  /* Set class request setup packet bRequest */
  g_usb_hsmpl_request.bRequestCode = USB_HID_GET_PHYSICAL_DESCRIPTOR;

  g_usb_hsmpl_request.tranadr     = p_data;                               /* Physical descriptor storage address */
  g_usb_hsmpl_request.tranlen     = length;                               /* Physical descriptor length */
  g_usb_hsmpl_request.index       = index;                                /* Descriptor index */
  g_usb_hsmpl_request.devadr      = usb_shhid_smpl_devaddr;               /* Device address */
  g_usb_hsmpl_request.complete    = (usb_cb_t)&usb_hsmpl_class_result;    /* Set callback function */

  /* Issue host HID class request */
  R_usb_hhid_class_request(&g_usb_hsmpl_request);
}

/******************************************************************************
Function Name   : usb_hhid_smpl_get_protocol
Description     : Host HID class request GetReport
Arguments       : uint16_t  *protocol        : Protocol(0:Boot,1:Report)storage address
Return value    : none
******************************************************************************/
USB_STATIC void usb_hhid_smpl_get_protocol(uint8_t *protocol)
{
  /* Class request setup packet bRequest Set */
  g_usb_hsmpl_request.bRequestCode    = USB_HID_GET_PROTOCOL;

  g_usb_hsmpl_request.tranadr     = protocol;                             /* Protocol code storage address */
  g_usb_hsmpl_request.devadr      = usb_shhid_smpl_devaddr;               /* Device address */
  g_usb_hsmpl_request.complete    = (usb_cb_t)&usb_hsmpl_class_result;    /* Set callback function */

  R_usb_hhid_class_request(&g_usb_hsmpl_request); /* Issue host HID class request */
}

/******************************************************************************
Function Name   : usb_hhid_smpl_set_idle
Description     : Host HID class request SetIdle
Arguments       : uint16_t  duration        : Report interval(4msec unit)
Return value    : none
******************************************************************************/
USB_STATIC void usb_hhid_smpl_set_idle(uint16_t duration)
{
  /* Class request setup packet bRequest Set */
  g_usb_hsmpl_request.bRequestCode = USB_HID_SET_IDLE;

  g_usb_hsmpl_request.tranadr     = USB_NULL;                             /* No-data control */
  g_usb_hsmpl_request.duration    = duration;                             /* Report interval */
  g_usb_hsmpl_request.devadr      = usb_shhid_smpl_devaddr;               /* Device address */
  g_usb_hsmpl_request.complete    = (usb_cb_t)&usb_hsmpl_class_result;    /* Set callback function */

  /* Issue host HID class request */
  R_usb_hhid_class_request(&g_usb_hsmpl_request);
}

/******************************************************************************
Function Name   : usb_hhid_smpl_get_idle
Description     : Host HID class request GetIdle
Arguments       : uint16_t  *idle_rate      : Report interval(4msec unit)storage address
Return value    : none
******************************************************************************/
USB_STATIC void usb_hhid_smpl_get_idle(uint16_t *idle_rate)
{
  /* Class request setup packet bRequest Set */
  g_usb_hsmpl_request.bRequestCode = USB_HID_GET_IDLE;

  g_usb_hsmpl_request.tranadr     = idle_rate;                            /* Report interval storage address */
  g_usb_hsmpl_request.devadr      = usb_shhid_smpl_devaddr;               /* Device address */
  g_usb_hsmpl_request.complete    = (usb_cb_t)&usb_hsmpl_class_result;    /* Set callback function */

  /* Issue host HID class request */
  R_usb_hhid_class_request(&g_usb_hsmpl_request);
}

#endif  /* CLASS_REQUEST_SAMPLE */

/******************************************************************************
End Of File
******************************************************************************/
