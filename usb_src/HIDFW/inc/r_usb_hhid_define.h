/******************************************************************************
* File Name     : r_usb_hhid_define.h
* Version       : 2.10
* Description   : USB Host HID class definition
******************************************************************************
* History   : DD.MM.YYYY Version Description
*           : 01.08.2013 2.10    Basic Mini FW Ver2.10 Release
******************************************************************************/
#ifndef __R_USB_HHID_DEFINE_H__
#define __R_USB_HHID_DEFINE_H__

/*****************************************************************************
Macro definitions
******************************************************************************/
/* ----- The API function type Defines -------- */
/* The API function type is conbined the class request code */
/* HID Class Request code  */
#define USB_HID_GET_REPORT              0x01u
#define USB_HID_GET_IDLE                0x02u
#define USB_HID_GET_PROTOCOL            0x03u
#define USB_HID_SET_IDLE                0x0Au
#define USB_HID_SET_REPORT              0x09u
#define USB_HID_SET_PROTOCOL            0x0Bu
/* HID API function type  */
#define USB_HID_GET_HID_DESCRIPTOR      0x21u
#define USB_HID_GET_REPORT_DESCRIPTOR   0x22u
#define USB_HID_GET_PHYSICAL_DESCRIPTOR 0x23u

/* HID Class Descriptor type */
#define USB_HID_HID_DESCRIPTOR          0x2100u
#define USB_HID_REPORT_DESCRIPTOR       0x2200u
#define USB_HID_PHYSICAL_DESCRIPTOR     0x2300u

/* ----- Subclass Codes InterfaceDescriptor[bInterfaceSubClass(6)]----- */
#define USB_HID_IFSUB_NONE              (uint8_t)0x00       /* No Subclass */
#define USB_HID_IFSUB_BOOT              (uint8_t)0x01       /* Boot Interface Subclass */

/* ----- Protocol Codes InterfaceDescriptor[bInterfaceProtocol(7)]----- */
#define USB_HID_IFPRO_NUM               7u                  /* InterfaceDescriptor[7] */
#define USB_HID_IFPRO_NONE              (uint8_t)0x00
#define USB_HID_IFPRO_KBD               (uint8_t)0x01       /* Keyboard */
#define USB_HID_IFPRO_MSE               (uint8_t)0x02       /* Mouse */

/* ----- Report Types ----- */
#define USB_HID_TYPE_INPUTREPORT        0x0100u             /* Report Type Input */
#define USB_HID_TYPE_OUTPUTREPORT       0x0200u             /* Report Type Output */
#define USB_HID_TYPE_FEATUREREPORT      0x0300u             /* Report Type Feature */

/* ----- Protocol Types ----- */
#define USB_HID_TYPE_BOOTPROTOCOL       (uint8_t)0          /* Boot Protocol */
#define USB_HID_TYPE_REPORTPROTOCOL     (uint8_t)1          /* Report Protocol */

/* Report Type set */
#define USB_HID_SHIFT2LEFT(v)           (uint16_t)((v) << 8)

/* Host HID Transfer Pipe */
#define USB_HHID_USE_PIPE               USB_PIPE6


/*****************************************************************************
Enumerated Types
******************************************************************************/
/* Host HID Task Command */
typedef enum
{
    USB_HHID_TCMD_OPEN,
    USB_HHID_TCMD_SEND,
    USB_HHID_TCMD_RECEIVE,
    USB_HHID_TCMD_CLASS_REQ,
    USB_HHID_TCMD_DATA_TRANS
} USB_HHID_TCMD_t;

/* Enumeration Sequence */
typedef enum
{
    USB_HHID_ENUM_COMPLETE = 0,         /* Complete */
    USB_HHID_ENUM_STR_DT0_REQ,          /* String Descriptor #0 receive request */
    USB_HHID_ENUM_STR_DT0_WAIT,         /* String Descriptor #0 Receive complete */
    USB_HHID_ENUM_STR_IPRODUCT_WAIT     /* iProduct Receive complete */
} USB_HHID_ENUM_SEQ_t;

/*****************************************************************************
Struct definition
******************************************************************************/
typedef struct
{
    usb_addr_t      devadr;
    uint16_t        bRequestCode;
    void            *tranadr;       /* Transfer data Start address */
    usb_leng_t      tranlen;        /* Transfer data length */
    uint16_t        index;
    uint16_t        duration;
    uint8_t         set_protocol;
    usb_cb_t        complete;       /* Call Back Function Info */
} USB_HHID_CLASS_REQUEST_PARM_t;


#endif  /* __R_USB_HHID_DEFINE_H__ */
/******************************************************************************
End Of File
******************************************************************************/
