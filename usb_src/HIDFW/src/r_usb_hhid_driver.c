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
* File Name     : r_usb_hhid_driver.c
* Version       : 2.10
* Description   : This is the USB host HID class driver function code.
*******************************************************************************/
/*******************************************************************************
* History   : DD.MM.YYYY Version Description
*           : 01.08.2013 2.10    Basic Mini FW Ver2.10 Release
*******************************************************************************/

/******************************************************************************
Section definitions
******************************************************************************/
#ifdef  R8CUSB
#pragma section program P_HID
#pragma section bss B_HID
#pragma section data R_HID
#endif
#ifdef  RL78USB
#endif
#ifdef  RL78USB_SECTION
#pragma section @@CODE   P_HID
#pragma section @@BASE   T_HID
#pragma section @@CNST   C_HID
#pragma section @@DATA   B_HID
#pragma section @@R_INIT D_HID
#pragma section @@INIT   R_HID
#endif
#ifdef  __RX
#pragma section = "_HID"
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


/******************************************************************************
Macro definitions
******************************************************************************/
#define USB_HHID_CLSDATASIZE            512


/******************************************************************************
Typedef definitions
******************************************************************************/
/* There is no typedef definition. */


/******************************************************************************
Private global variables and functions
******************************************************************************/
/* variables */
USB_STATIC uint8_t          usb_shhid_str_desc_data[USB_HHID_CLSDATASIZE];
USB_STATIC usb_utr_t        usb_shhid_class_control;                /* Request for HID to USB Request */
USB_STATIC usb_hcdrequest_t g_usb_hhid_Request;                     /* Control Transfer Request field */
USB_STATIC usb_cb_t         usb_shhid_data_tran_complete;
USB_STATIC const uint16_t   usb_gapl_devicetpl[] =                  /* Host HID TPL */
{
  1,                  /* Number of list */
  0,                  /* Reserved */
  USB_NOVENDOR,       /* Vendor ID  : no-check */
  USB_NOPRODUCT,      /* Product ID : no-check */
};

/* functions */
USB_STATIC void     usb_hhid_enumeration_sequence(usb_utr_t *mess);
USB_STATIC void     usb_hhid_InTransferResult(usb_utr_t *mess);
USB_STATIC uint16_t usb_hhid_pipe_info(uint8_t *table, uint16_t length);
USB_STATIC usb_er_t usb_hhid_class_request_process(void);
USB_STATIC void     usb_hhid_get_string_desc(usb_addr_t addr, uint16_t string, usb_cb_t complete);
USB_STATIC void     usb_cstd_AnsiCallBack(usb_utr_t *ptr);


/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
/* variables */
USB_HHID_CLASS_REQUEST_PARM_t   usb_ghhid_class_request_parm;   /* Host HID Class Request Parametor */
usb_utr_t       usb_shhid_transfer_message;                     /* USB data Transfer Request for HCD */
uint8_t         *usb_shhid_device_table;                        /* Device Descriptor Table */
uint8_t         *usb_ghhid_ConfigTable;                         /* Configuration Descriptor Table */
uint8_t         *usb_ghhid_interface_table;                     /* Interface Descriptor Table */
usb_addr_t      usb_ghhid_Devaddr;                              /* Device Address */
uint16_t        usb_ghhid_maxps;                                /* Max Packet Size */
uint16_t        usb_shhid_enum_seq;                             /* Enumeration Sequence */
uint16_t usb_ghhid_SmplEpTbl[] =
{
  USB_HHID_USE_PIPE,
  /* TYPE  / BFRE     / DBLB   / CNTMD    / SHTNAK        / DIR     / EPNUM */
  USB_NULL | USB_BFREOFF | USB_NULL | USB_NULL | USB_SHTNAKOFF | USB_NULL | USB_NULL,
  USB_NULL,
  USB_NULL,
  /* Pipe end */
  USB_PDTBLEND,
};

/* functions */
void        usb_hhid_initialize(void);
void        usb_hhid_task(void);
void        usb_hhid_dummy_function(void);


/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
/* There is no imported global variables and functions. */


/******************************************************************************
Renesas Host HID Driver functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_hhid_initialize
Description     : PHID sample Initialize process
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hhid_initialize( void )
{
  //  All global variables are initialized specifying it. 
  //  However, an unnecessary variable to initialize ends comment by "//". 
  //unnecessary   usb_shhid_str_desc_data[];
  //unnecessary   g_usb_hhid_Request[];
  //unnecessary   usb_shhid_data_tran_complete;
  //unnecessary   usb_ghhid_class_request_parm;
  //unnecessary   usb_ghhid_ConfigTable;
  //unnecessary   usb_ghhid_interface_table;
  //unnecessary   usb_ghhid_maxps;
  usb_shhid_class_control.pipectr     = USB_NULL;
  usb_shhid_transfer_message.pipectr  = USB_NULL;
  usb_ghhid_Devaddr       = USB_DEVICE_0;
  usb_shhid_enum_seq      = (uint16_t)USB_HHID_ENUM_STR_DT0_REQ;
  
}   /* eof usb_hhid_initialize() */


/******************************************************************************
Function Name   : usb_hhid_task
Description     : Host HIT Task
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hhid_task(void)
{
  usb_utr_t   *mess;
  usb_er_t    err;
  
  err = R_USB_RCV_MSG(USB_HHID_MBX,(usb_msg_t**)&mess);
  if (err == USB_E_OK)
  {
    switch (mess->msginfo)
    {
    case USB_HHID_TCMD_OPEN:
      usb_hhid_enumeration_sequence(mess);
      break;
      
    case USB_HHID_TCMD_DATA_TRANS:
      /* data Receive request */
      usb_shhid_data_tran_complete        = mess->complete;
      usb_shhid_transfer_message.pipenum   = mess->pipenum;
      usb_shhid_transfer_message.complete = (usb_cb_t)&usb_hhid_InTransferResult;
      err =R_usb_hstd_TransferStart(&usb_shhid_transfer_message);
      /* Transfer Start request check */
      if (err != USB_E_OK)
      {
#ifdef  __CA78K0R__
        ((void (*)(usb_utr_t*))*mess->complete)(mess);  /* for CA78K0R compiler */
#else
        (*mess->complete)(mess);
#endif
      }
      break;
      
    case USB_HHID_TCMD_CLASS_REQ:
      err = usb_hhid_class_request_process();
      break;
    default:
      break;
    }
    err = R_USB_REL_BLK(mess->flag);
    if (err != USB_E_OK)
    {
    }
  }
  else
  {
    return;
  }
}   /* eof usb_hhid_task() */


/******************************************************************************
Function Name   : usb_hhid_EnumerationSequence
Description     :
Arguments       : usb_utr_t *mess
Return value    : none
******************************************************************************/
USB_STATIC void usb_hhid_enumeration_sequence(usb_utr_t *mess)
{
  uint16_t    retval;
  uint16_t    string;
  uint8_t     *p_desc;
  uint8_t     *p_iftable;
  uint16_t    desc_len;
  totallen_t  device_id, length2;
  uint16_t    length1;
  uint8_t     i;
  
  /* Branch for Enumeration Sequence */
  switch (usb_shhid_enum_seq)
  {
    /* Enumeration Sequence String Descriptor #0 receive request */
  case    USB_HHID_ENUM_STR_DT0_REQ:
    device_id.WORD.BYTE.up      = usb_shhid_device_table[USB_DEV_VENDOR_HI];
    device_id.WORD.BYTE.dn      = usb_shhid_device_table[USB_DEV_VENDOR_LO];
    
    /* Check TPL */
    for (i = 0, retval = USB_ERROR; i < usb_gapl_devicetpl[0]; i++)
    {
      if ((usb_gapl_devicetpl[(i * 2) + 2] == USB_NOVENDOR) ||
          (usb_gapl_devicetpl[(i * 2) + 2] == device_id.WORD.w))
      {
        device_id.WORD.BYTE.up      = usb_shhid_device_table[USB_DEV_VENDOR_HI];
        device_id.WORD.BYTE.dn      = usb_shhid_device_table[USB_DEV_VENDOR_LO];
        if ((usb_gapl_devicetpl[(i * 2) + 3] == USB_NOPRODUCT) ||
            (usb_gapl_devicetpl[(i * 2) + 3] == device_id.WORD.w))
        {
          retval = USB_YES;
        }
      }
    }
    if (retval == USB_ERROR)
    {
      R_usb_hstd_ReturnEnuMGR(USB_NO);
      break;
    }
    
    /* Check interface class */
    length1 = 0;
    length2.WORD.BYTE.up        = usb_ghhid_ConfigTable[USB_CON_TOTALLENG_HI];
    length2.WORD.BYTE.dn        = usb_ghhid_ConfigTable[USB_CON_TOTALLENG_LO];
    if (length2.WORD.w > USB_CONFIGSIZE)
    {
      length2.WORD.w = USB_CONFIGSIZE;
    }
    
    /* Search within configuration total-length */
    retval = USB_ERROR;
    while (length1 < length2.WORD.w)
    {
      if (usb_ghhid_ConfigTable[length1+1] == USB_DT_INTERFACE)
      {
        usb_ghhid_interface_table   = (uint8_t*)&usb_ghhid_ConfigTable[length1];
        /* CLASSE: HUMAN INTERFACE DEVICE */
        if ((uint16_t)usb_ghhid_ConfigTable[length1+USB_INTF_CLASS] == USB_IFCLS_HID)
        {
          retval = USB_YES;
          /* Get String descriptor */
          usb_hhid_get_string_desc(usb_ghhid_Devaddr, (uint16_t)0, (usb_cb_t)&usb_cstd_AnsiCallBack);
          /* String Descriptor #0 Receive wait */
          usb_shhid_enum_seq++;
          break;
        }
//        else
//        {
//          /* CLASSE: MASS STORAGE DEVICE */
//          if ((uint16_t)usb_ghhid_ConfigTable[length1+USB_INTF_CLASS] == USB_IFCLS_MAS)
//          {
//            retval = USB_YES;
//            /* Get String descriptor */
//            usb_hhid_get_string_desc(usb_ghhid_Devaddr, (uint16_t)0, (usb_cb_t)&usb_cstd_AnsiCallBack);
//            /* String Descriptor #0 Receive wait */
//            usb_shhid_enum_seq++;
//            break;
//          }
//        }
      }
      length1 += usb_ghhid_ConfigTable[length1];
    }
    if (retval == USB_ERROR)
    {
      R_usb_hstd_ReturnEnuMGR(USB_NO);
    }
    break;
    
    /* Enumeration Sequence String Descriptor #0 Receive complete */
  case    USB_HHID_ENUM_STR_DT0_WAIT:
    /* String descriptor check */
    if (mess->status==(usb_er_t)USB_CTRL_END)
    {
      string  = usb_shhid_device_table[USB_DEV_IPRODUCT];             /* Check iProduct */
      /* Get String descriptor */
      usb_hhid_get_string_desc(usb_ghhid_Devaddr, string, (usb_cb_t)&usb_cstd_AnsiCallBack);
    }
    /* StringDescriptor iProduct Receive */
    usb_shhid_enum_seq++;
    break;
    
    /* StringDescriptor iProduct Receive complete */
  case    USB_HHID_ENUM_STR_IPRODUCT_WAIT:
    /* String descriptor check */
    if (mess->status==(usb_er_t)USB_CTRL_END)
    {
    }
    else
    {
    }
    
    /* Check protocol (bInterfaceProtocol - Interface Descriptor)*/
    p_desc = usb_ghhid_ConfigTable;
    desc_len = (uint16_t)(((uint16_t)*(p_desc + 3)) << 8);
    desc_len += (uint16_t)*(p_desc + 2);
    
    // Searching InterfaceDescriptor
    p_iftable = usb_ghhid_interface_table;
    desc_len = (uint16_t)(desc_len - (p_iftable - p_desc));
    
    if ((p_iftable[USB_HID_IFPRO_NUM] != USB_HID_IFPRO_KBD) &&
        (p_iftable[USB_HID_IFPRO_NUM] != USB_HID_IFPRO_MSE))
    {
      /* Enumeration Sequence Complete */
      usb_shhid_enum_seq  = USB_HHID_ENUM_COMPLETE;
      /* Enumeration class check error */
      R_usb_hstd_ReturnEnuMGR(USB_NO);
      break;
    }
    
    /* pipe information table set */
    retval = usb_hhid_pipe_info(p_iftable, desc_len);
    if (retval == USB_ERROR)
    {
      /* Enumeration Sequence Complete */
      usb_shhid_enum_seq  = USB_HHID_ENUM_COMPLETE;
      /* Enumeration class check error */
      retval  = USB_NO;
    }
    else
    {
      /* Enumeration Sequence Complete */
      usb_shhid_enum_seq  = USB_HHID_ENUM_COMPLETE;
      /* Enumeration class check OK */
      retval  = USB_YES;
    }
    /* return to MGR */
    R_usb_hstd_ReturnEnuMGR(retval);
    break;
  default:
    break;
  }
}   /* eof usb_hhid_EnumerationSequence() */


/******************************************************************************
Function Name   : usb_hhid_InTransferResult
Description     : data Transfer
Arguments       : usb_utr_t *mess       : Data transfer message pointer
Return value    : none
******************************************************************************/
USB_STATIC void usb_hhid_InTransferResult(usb_utr_t *mess)
{
#ifdef  __CA78K0R__
  ((void (*)(usb_utr_t*))*usb_shhid_data_tran_complete)(mess);
#else
  (*usb_shhid_data_tran_complete)(mess);
#endif
}   /* eof usb_hhid_InTransferResult() */


/******************************************************************************
Function Name   : usb_hhid_pipe_info
Description     : pipe information table set
Arguments       : uint8_t  *table       : Descriptor store address
: uint16_t length       : Descriptor total lenght
Return value    : uint16_t              : Error info
******************************************************************************/
USB_STATIC uint16_t usb_hhid_pipe_info(uint8_t *table, uint16_t length)
{
  /* Offset for Descriptor Top Address */
  uint16_t        ofdsc;
  uint16_t        retval;
  
  /* Check configuration descriptor */
  if (table[1] != USB_DT_INTERFACE)
  {
    return  USB_ERROR;
  }
  
  /* Descriptor Address set */
  ofdsc = table[0];
  /* Loop for Endpoint Descriptor search */
  while (ofdsc < length)
  {
    /* Branch descriptor type */
    switch (table[ofdsc + 1])
    {
    case USB_DT_DEVICE:
      /* Continue */
    case USB_DT_CONFIGURATION:
      /* Continue */
    case USB_DT_STRING:
      /* Continue */
    case USB_DT_INTERFACE:
      return USB_ERROR;
      break;
      
      /* Endpoint Descriptor */
    case USB_DT_ENDPOINT:
      /* Check pipe information */
      retval = R_usb_hstd_ChkPipeInfo((uint16_t*)&usb_ghhid_SmplEpTbl, (uint8_t*)&table[ofdsc]);
      //@@@ The retrieval is continued until the descriptor is corresponding to the pipe information.
      if (retval == USB_DIR_H_IN)
      {
        /* Get Max Packet size */
        usb_ghhid_maxps = (uint16_t)table[ofdsc + 4];
        usb_ghhid_maxps |= (uint16_t)table[ofdsc + 5] << 8;
        return USB_YES;
      }
      /* Next descriptor point set */
      ofdsc   += table[ofdsc];
      break;
      
    case USB_DT_DEVICE_QUALIFIER:
      /* Continue */
    case USB_DT_OTHER_SPEED_CONF:
      /* Continue */
    case USB_DT_INTERFACE_POWER:
      return USB_ERROR;
      break;
      
    default:
      /* Next descriptor point set */
      ofdsc   +=  table[ofdsc];
      break;
    }
  }
  //@@@ The error responds when a corresponding descriptor to the pipe information is not found.
  return USB_ERROR;
}   /* eof usb_hhid_pipe_info() */


/******************************************************************************
Function Name   : usb_hhid_class_request_process
Description     : Host HID Class Request Process
Arguments       : none
Return value    : usb_er_t               : USB_ERROR/USB_DONE
******************************************************************************/
USB_STATIC usb_er_t usb_hhid_class_request_process(void)
{
  usb_er_t        err;
  
  switch (usb_ghhid_class_request_parm.bRequestCode)
  {
  case USB_HID_GET_HID_DESCRIPTOR:
    g_usb_hhid_Request.WORD.BYTE.bmRequestType  = USB_REQUEST_TYPE(USB_DEV_TO_HOST,USB_STANDARD,USB_INTERFACE);
    g_usb_hhid_Request.WORD.BYTE.bRequest       = USB_GET_DESCRIPTOR;
    g_usb_hhid_Request.wValue   = USB_HID_HID_DESCRIPTOR;
    g_usb_hhid_Request.wIndex   = 0x0000;
    g_usb_hhid_Request.wLength  = 9;
    break;
  case USB_HID_GET_REPORT_DESCRIPTOR:
    g_usb_hhid_Request.WORD.BYTE.bmRequestType  = USB_REQUEST_TYPE(USB_DEV_TO_HOST,USB_STANDARD,USB_INTERFACE);
    g_usb_hhid_Request.WORD.BYTE.bRequest       = USB_GET_DESCRIPTOR;
    g_usb_hhid_Request.wValue   = USB_HID_REPORT_DESCRIPTOR;
    g_usb_hhid_Request.wIndex   = 0x0000;
    g_usb_hhid_Request.wLength  = usb_ghhid_class_request_parm.tranlen;
    break;
  case USB_HID_GET_PHYSICAL_DESCRIPTOR:   /* Setup message bmRequestType and bRequest set */
    g_usb_hhid_Request.WORD.BYTE.bmRequestType  = USB_REQUEST_TYPE(USB_DEV_TO_HOST,USB_STANDARD,USB_INTERFACE);
    g_usb_hhid_Request.WORD.BYTE.bRequest       = USB_GET_DESCRIPTOR;
    g_usb_hhid_Request.wValue   = (uint16_t)(USB_HID_PHYSICAL_DESCRIPTOR | usb_ghhid_class_request_parm.index);
    g_usb_hhid_Request.wIndex   = 0x0000;
    g_usb_hhid_Request.wLength  = (uint16_t)usb_ghhid_class_request_parm.tranlen;
    break;
    
  case USB_HID_SET_REPORT:
    g_usb_hhid_Request.WORD.BYTE.bmRequestType  = USB_REQUEST_TYPE(USB_HOST_TO_DEV,USB_CLASS,USB_INTERFACE);
    g_usb_hhid_Request.WORD.BYTE.bRequest       = USB_HID_SET_REPORT;
    g_usb_hhid_Request.wValue   = USB_HID_TYPE_OUTPUTREPORT;                /* No report ID */
    g_usb_hhid_Request.wIndex   = 0x0000;
    g_usb_hhid_Request.wLength  = (uint16_t)usb_ghhid_class_request_parm.tranlen;
    break;
  case USB_HID_GET_REPORT:
    g_usb_hhid_Request.WORD.BYTE.bmRequestType  = USB_REQUEST_TYPE(USB_DEV_TO_HOST,USB_CLASS,USB_INTERFACE);
    g_usb_hhid_Request.WORD.BYTE.bRequest       = USB_HID_GET_REPORT;
    g_usb_hhid_Request.wValue   = USB_HID_TYPE_INPUTREPORT;
    g_usb_hhid_Request.wIndex   = 0x0000;
    g_usb_hhid_Request.wLength  = (uint16_t)usb_ghhid_class_request_parm.tranlen;
    break;
    
  case USB_HID_SET_IDLE:
    g_usb_hhid_Request.WORD.BYTE.bmRequestType  = USB_REQUEST_TYPE(USB_HOST_TO_DEV,USB_CLASS,USB_INTERFACE);
    g_usb_hhid_Request.WORD.BYTE.bRequest       = USB_HID_SET_IDLE;
    g_usb_hhid_Request.wValue   = USB_HID_SHIFT2LEFT(usb_ghhid_class_request_parm.duration);
    g_usb_hhid_Request.wIndex   = 0x0000;
    g_usb_hhid_Request.wLength  = 0;
    break;
  case USB_HID_GET_IDLE:
    g_usb_hhid_Request.WORD.BYTE.bmRequestType  = USB_REQUEST_TYPE(USB_DEV_TO_HOST,USB_CLASS,USB_INTERFACE);
    g_usb_hhid_Request.WORD.BYTE.bRequest       = USB_HID_GET_IDLE;
    g_usb_hhid_Request.wValue   = 0;
    g_usb_hhid_Request.wIndex   = 0x0000;
    g_usb_hhid_Request.wLength  = 1;
    break;
    
  case USB_HID_SET_PROTOCOL://marcos
    g_usb_hhid_Request.WORD.BYTE.bmRequestType  = USB_REQUEST_TYPE(USB_HOST_TO_DEV,USB_CLASS,USB_INTERFACE);
    g_usb_hhid_Request.WORD.BYTE.bRequest       = USB_HID_SET_PROTOCOL;
    g_usb_hhid_Request.wValue   = (uint16_t)usb_ghhid_class_request_parm.set_protocol;
    g_usb_hhid_Request.wIndex   = 0x0000;
    g_usb_hhid_Request.wLength  = 0;
    break;
  case USB_HID_GET_PROTOCOL:
    g_usb_hhid_Request.WORD.BYTE.bmRequestType  = USB_REQUEST_TYPE(USB_DEV_TO_HOST,USB_CLASS,USB_INTERFACE);
    g_usb_hhid_Request.WORD.BYTE.bRequest       = USB_HID_GET_PROTOCOL;
    g_usb_hhid_Request.wValue   = 0;
    g_usb_hhid_Request.wIndex   = 0x0000;
    g_usb_hhid_Request.wLength  = 1;
    break;
    
  default:
    break;
  }
  
  usb_shhid_class_control.tranadr     = usb_ghhid_class_request_parm.tranadr;
  usb_shhid_class_control.complete    = usb_ghhid_class_request_parm.complete;
  usb_shhid_class_control.tranlen     = g_usb_hhid_Request.wLength;
  usb_shhid_class_control.pipenum     = USB_PIPE0;
  usb_shhid_class_control.setup       = (uint16_t*)&g_usb_hhid_Request;
  g_usb_hhid_Request.Address          = (uint16_t)usb_ghhid_class_request_parm.devadr;
  
  err = R_usb_hstd_TransferStart(&usb_shhid_class_control);
  
  return err;
}   /* eof usb_hhid_class_request_process() */


/******************************************************************************
Function Name   : usb_hhid_get_string_desc
Description     : Set GetDescriptor
Arguments       : usb_addr_t addr         : device address
: uint16_t string         : descriptor index
: usb_cb_t complete     : callback function
Return value    : none
******************************************************************************/
USB_STATIC void usb_hhid_get_string_desc(usb_addr_t addr, uint16_t string, usb_cb_t complete)
{
  uint16_t        i;
  
  if (string == 0)
  {
    g_usb_hhid_Request.wIndex   = 0;;
    g_usb_hhid_Request.wLength  = 4;
  }
  else
  {
    g_usb_hhid_Request.wIndex   = (uint16_t)(usb_shhid_str_desc_data[2]);
    g_usb_hhid_Request.wIndex   |= (uint16_t)((uint16_t)(usb_shhid_str_desc_data[3]) << 8);
    g_usb_hhid_Request.wLength  = (uint16_t)USB_HHID_CLSDATASIZE;
  }
  g_usb_hhid_Request.WORD.BYTE.bmRequestType  = USB_REQUEST_TYPE(USB_DEV_TO_HOST,USB_STANDARD,USB_DEVICE);
  g_usb_hhid_Request.WORD.BYTE.bRequest       = USB_GET_DESCRIPTOR;
  g_usb_hhid_Request.wValue                   = (uint16_t)(USB_STRING_DESCRIPTOR + string);
  g_usb_hhid_Request.Address                  = (uint16_t)addr;
  
  for (i = 0; i < g_usb_hhid_Request.wLength; i++)
  {
    usb_shhid_str_desc_data[i] = (uint8_t)0xFF;
  }
  
  usb_shhid_class_control.tranadr     = (void*)usb_shhid_str_desc_data;
  usb_shhid_class_control.complete    = complete;
  usb_shhid_class_control.tranlen     = (usb_leng_t)g_usb_hhid_Request.wLength;
  usb_shhid_class_control.pipenum     = USB_PIPE0;
  usb_shhid_class_control.setup       = (uint16_t*)&g_usb_hhid_Request;
  
  R_usb_hstd_TransferStart(&usb_shhid_class_control);
}   /* eof usb_hhid_get_string_desc() */


/******************************************************************************
Function Name   : usb_cstd_AnsiCallBack
Description     :
Arguments       :
Return value    :
******************************************************************************/
USB_STATIC void usb_cstd_AnsiCallBack(usb_utr_t *ptr)
{
  ptr->msginfo    = USB_HHID_TCMD_OPEN;
  R_USB_SND_MSG(USB_HHID_MBX, (usb_msg_t*)ptr);
}   /* eof usb_cstd_AnsiCallBack() */


/******************************************************************************
Function Name   : usb_hhid_dummy_function
Description     : HID dummy function
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hhid_dummy_function(void)
{
}   /* eof usb_hhid_dummy_function() */


/******************************************************************************
End Of File
******************************************************************************/
