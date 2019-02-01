/*******************************************************************************
* File Name    : r_usb_usrconfig.h
* Version      : 2.10
* Description  : Definition of user macro
*******************************************************************************/
/*******************************************************************************
* History   : DD.MM.YYYY Version Description
*           : 31.03.2012 1.80    Basic FW mini Ver2.00(ES Version)
*           : 30.11.2012 2.00    Basic FW mini Ver2.00 Release
*           : 31.01.2013 2.01    Corresponded to RL78/L1C
*           : 01.08.2013 2.10    Basic Mini FW Ver2.10 Release
*******************************************************************************/
#ifndef __R_USB_USRCONFIG_H__
#define __R_USB_USRCONFIG_H__

#include <iorx111.h>
#include <r_cg_userdefine.h>
/******************************************************************************
Macro definitions (for Preprocessor)
******************************************************************************/
#define USB_HOST_PP                 (1)
#define USB_PERI_PP                 (2)

#define USB_1PORT_PP                (1)
#define USB_2PORT_PP                (2)

#define USB_PIPE_FULL               (1)


/******************************************************************************
Macro definitions (for User define)
******************************************************************************/

/* H/W function type */
#define USB_HOST                ((uint16_t)1)       /* Host mode */
#define USB_PERI                ((uint16_t)2)       /* Peripheral mode */
#define USB_HOST_PERI           ((uint16_t)3)       /* Host/Peri mode */
#define USB_OTG                 ((uint16_t)4)       /* Otg mode */


/* The user edits the rear side here. */
/******************************************************************************
Macro definitions (597IP: To define the USB operation mode type)
******************************************************************************/
/*  Set the USB mode. (The USB mode specified in the project file.) */
//  #define USB_FUNCSEL_PP      USB_HOST_PP
//  #define USB_FUNCSEL_PP      USB_PERI_PP

/*  Set the transfer rate when the peripheral operation is selected. */
/* (The transfer rate is specified in the project file.) */
#if USB_FUNCSEL_PP == USB_PERI_PP
//  #define USB_LSPERI_PP
//  #define USB_FSPERI_PP
#endif  /* USB_PERI_PP */

/*  Set the port number when the host operation is selected. */
/* (The port number is selected in the project file.) */
#if USB_FUNCSEL_PP == USB_HOST_PP
#ifdef RL78G1C
//  #define USB_PORTSEL_PP      USB_1PORT_PP
  #define USB_PORTSEL_PP      USB_2PORT_PP
#else
  #define USB_PORTSEL_PP      USB_1PORT_PP
//  #define USB_PORTSEL_PP      USB_2PORT_PP
#endif
#endif  /* USB_HOST_PP */


#define USB_STATIC_USE
#define USB_DEBUG_HOOK_USE
//#define USB_HOST_BC_ENABLE

#ifdef USB_STATIC_USE
  #define USB_STATIC  static
#else
  #define USB_STATIC
#endif  /* USB_STATIC_USE */

/* DTC DEFINE */
#define DTC_USE_PIPE_NUM    USB_NOUSE
//#define DTC_USE_PIPE_NUM    USB_PIPE1
//#define DTC_USE_PIPE_NUM    USB_PIPE2
//#define DTC_USE_PIPE_NUM    USB_PIPE3
//#define DTC_USE_PIPE_NUM    USB_PIPE4
//#define DTC_USE_PIPE_NUM    USB_PIPE5

/******************************************************************************
Macro definitions (MCU type)
******************************************************************************/
/*  Set the MCU type. (The MCU type specified in the project file.) */
//  #define R8CUSB
//  #define RL78USB
//  #define RL78USB_SECTION

/*  Set the family type when the RL78 is selected by the MCU type. */
/* (The family type is specified in the project file.) */
//  #define RL78G1C
//  #define RL78L1C


/* USB register base address */  
  #define   USB_IP  (*(volatile struct st_usb0 __evenaccess *)0xA0000)
/******************************************************************************
Macro definitions (User define)
******************************************************************************/

/* Interrupt message num */
#define USB_INTMSGMAX           ((uint16_t)10)


/******************************************************************************
Macro definitions (User define PERIPHERAL)
******************************************************************************/

    #define USB_STRINGNUM       (7u)            /* Max of string descriptor */


/******************************************************************************
Macro definitions (User define HOST)
******************************************************************************/

/* Max pipe error count */
    #define USB_PIPEERROR       (1u)

/* Descriptor size */
    #define USB_DEVICESIZE      (18u)   /* Device Descriptor size */
    #define USB_CONFIGSIZE      (256u)  /* Configuration Descriptor size */

/* USB Device address define */
    #define USB_DEVICEADDR      (3u)    /* PORT0 USB Address (1 to 10) */

/* USB bus reset signal output waiting time */
    #define USB_TATTDB          3000    /* (Chapter 7.1.7.3 TATTDB) minimum duration of 100 ms */

/******************************************************************************
Macro definitions (User define HOST)
******************************************************************************/




#endif  /* __R_USB_USRCONFIG_H__ */
/******************************************************************************
End of file
******************************************************************************/
