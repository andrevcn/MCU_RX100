/*******************************************************************************
* File Name    : r_usb_ckernelid.h
* Version      : 2.10
* Description  : Definition of system call macro & kernel ID
*******************************************************************************/
/*******************************************************************************
* History   : DD.MM.YYYY Version Description
*           : 31.03.2012 1.80    Basic FW mini Ver2.00(ES Version)
*           : 30.11.2012 2.00    Basic FW mini Ver2.00 Release
*           : 31.01.2013 2.01    Corresponded to RL78/L1C
*           : 01.08.2013 2.10    Basic Mini FW Ver2.10 Release
*******************************************************************************/
#ifndef __R_USB_CKERNELID_H__
#define __R_USB_CKERNELID_H__


/******************************************************************************
Typedef definitions
******************************************************************************/
typedef struct
{
  void*               msghead;            /* Message header */
} usb_msg_t;
typedef int8_t          usb_er_t;           /* System call err */
typedef struct
{
  usb_msg_t   *Address;                   /* Waiting message */
  uint16_t    Counter;                    /* Wait counter */
} usb_waitinfo_t;
typedef struct
{
  usb_msg_t   *Address;                   /* Queuing as for some message. */
  uint8_t     TaskID;                     /* Wait task id */
} usb_waitque_t;


/******************************************************************************
Constant macro definitions
******************************************************************************/
#define USB_E_QOVR      ((int8_t)-43)       /* Queuing over flow */
#define USB_E_ERROR     ((int8_t)-1)        /* Error end */
#define USB_E_OK        ((int8_t)0)         /* Normal end */

/******************************************************************************
System call macro definitions
******************************************************************************/
#define R_USB_SND_MSG(ID, MESS)             R_usb_cstd_SndMsg( (uint8_t)ID, (usb_msg_t*)MESS )
#define R_USB_ISND_MSG(ID, MESS)            R_usb_cstd_iSndMsg( (uint8_t)ID, (usb_msg_t*)MESS )
#define R_USB_WAI_MSG(ID, MESS, CNT)        R_usb_cstd_WaiMsg( (uint8_t)ID, (usb_msg_t*)MESS, (uint16_t)CNT )
#define R_USB_RCV_MSG(ID, MESS)             R_usb_cstd_RecMsg( (uint8_t)ID, (usb_msg_t**)MESS )
#define R_USB_GET_SND(ID, MSG, CLB, KEY)    R_usb_cstd_PgetSend( (uint8_t)ID, (usb_strct_t)MSG, \
(usb_cbinfo_t) CLB, (usb_strct_t)KEY )
#define R_USB_REL_BLK(BLK)                  R_usb_cstd_RelBlk( (uint8_t)BLK )

/******************************************************************************
Scheduler use define
******************************************************************************/
#define USB_FLGCLR          ((uint8_t)0)            /* Flag clear */
#define USB_FLGSET          ((uint8_t)1)            /* Flag set */
#define USB_IDCLR           ((uint8_t)0xFF)         /* Priority clear */

/* Task ID define */
#define USB_TID_0           ((uint8_t)0)            /* Task ID 0 */
#define USB_TID_1           ((uint8_t)1)            /* Task ID 1 */
#define USB_TID_2           ((uint8_t)2)            /* Task ID 2 */
#define USB_TID_3           ((uint8_t)3)            /* Task ID 3 */
#define USB_TID_4           ((uint8_t)4)            /* Task ID 4 */

/* Please set with user system */
#define USB_IDMAX           ((uint8_t)5)            /* Maximum Task ID +1 */
#define USB_TABLEMAX        ((uint8_t)10)           /* Maximum priority table */
#define USB_BLKMAX          ((uint8_t)5)            /* Maximum block */

/* Peripheral Control Driver Task */
#define USB_PCD_TSK     USB_TID_0               /* Task ID */
#define USB_PCD_MBX     USB_PCD_TSK             /* Mailbox ID */
/* Peripheral Sample Vendor Class Driver Task */
#define USB_PVEN_TSK    USB_TID_3               /* Task ID */
#define USB_PVEN_MBX    USB_PVEN_TSK            /* Mailbox ID */
/* Peripheral Sample Application Task */
#define USB_PSMP_TSK    USB_TID_4               /* Task ID */
#define USB_PSMP_MBX    USB_PSMP_TSK            /* Mailbox ID */

/* Peripheral HID Class Driver Task */
#define USB_PHID_TSK    USB_TID_1               /* Task ID */
#define USB_PHID_MBX    USB_PHID_TSK            /* Mailbox ID */
/* Peripheral Sample Task */
#define USB_PHIDSMP_TSK USB_TID_2               /* Task ID */
#define USB_PHIDSMP_MBX USB_PHIDSMP_TSK         /* Mailbox ID */

/* PCDC Driver Task */
#define USB_PCDC_TSK    USB_TID_1               /* Task ID */
#define USB_PCDC_MBX    USB_PCDC_TSK            /* Mailbox ID */

/* Host Control Driver Task */
#define USB_HCD_TSK     USB_TID_0               /* Task ID */
#define USB_HCD_MBX     USB_HCD_TSK             /* Mailbox ID */
/* Host Manager Task */
#define USB_MGR_TSK     USB_TID_1               /* Task ID */
#define USB_MGR_MBX     USB_MGR_TSK             /* Mailbox ID */
/* Host Sample Vendor Class Driver Task */
#define USB_HVEN_TSK    USB_TID_2               /* Task ID */
#define USB_HVEN_MBX    USB_HVEN_TSK            /* Mailbox ID */
/* Host Sample Task */
#define USB_HSMP_TSK    USB_TID_3               /* Task ID */
#define USB_HSMP_MBX    USB_HSMP_TSK            /* Mailbox ID */

/* Host HID Class Driver Task */
#define USB_HHID_TSK    USB_TID_2               /* Task ID */
#define USB_HHID_MBX    USB_HHID_TSK            /* Mailbox ID */
/* Host CDC Class Driver Task */
#define USB_HCDC_TSK    USB_TID_2               /* Task ID */
#define USB_HCDC_MBX    USB_HCDC_TSK            /* Mailbox ID */
/* Host Sample Task */
#define USB_HCDCSMP_TSK USB_TID_3               /* Task ID */
#define USB_HCDCSMP_MBX USB_HCDCSMP_TSK         /* Mailbox ID */

/* Host Manager Task */
#define USB_MGR_TSK     USB_TID_1               /* Task ID */
#define USB_MGR_MBX     USB_MGR_TSK             /* Mailbox ID */

/* Host Sample Task */
#define USB_HMSC_TSK        USB_TID_2           /* Task ID */
#define USB_HMSC_MBX        USB_HMSC_TSK        /* Mailbox ID */

/* Host Sample Task */
#define USB_HSTRG_TSK       USB_TID_3           /* Task ID */
#define USB_HSTRG_MBX       USB_HSTRG_TSK       /* Mailbox ID */

/* Host Sample Task */
#define USB_HMSCSMP_TSK     USB_TID_4           /* Task ID */
#define USB_HMSCSMP_MBX     USB_HMSCSMP_TSK     /* Mailbox ID */


#endif  /* __R_USB_CKERNELID_H__ */
/******************************************************************************
End of file
******************************************************************************/
