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
* File Name     : r_usb_hhid_api.c
* Version       : 2.10
* Description   : USB host HID class driver API function code.
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
/* There is no macro definition. */


/******************************************************************************
Typedef definitions
******************************************************************************/
/* There is no typedef definition. */


/******************************************************************************
Private global variables and functions
******************************************************************************/
/* There is no orivate global variables and functions. */


/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
/* Exported global variables and functions are declared with the header file. */


/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
/* variables */
extern USB_HHID_CLASS_REQUEST_PARM_t    usb_ghhid_class_request_parm;
extern usb_utr_t    usb_shhid_transfer_message;     /* USB data Transfer Request for HCD */
extern uint8_t      *usb_shhid_device_table;        /* Device Descriptor Table */
extern uint8_t      *usb_ghhid_ConfigTable;         /* Configuration Descriptor Table */
extern uint8_t      *usb_ghhid_interface_table;     /* Interface Descriptor Table */
extern usb_addr_t   usb_ghhid_Devaddr;              /* Device Address */
extern uint16_t     usb_ghhid_maxps;                /* Max Packet Size */
extern uint16_t     usb_shhid_enum_seq;             /* Enumeration Sequence */
extern uint16_t     usb_ghhid_SmplEpTbl[];

/* functions */
extern void     usb_hhid_initialize(void);
extern void     usb_hhid_task(void);
extern void     usb_hhid_dummy_function(void);


/******************************************************************************
Renesas Abstracted USB Driver functions
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_hhid_task
Description     : Host HIT Task
Arguments       : none
Return value    : none
******************************************************************************/
void R_usb_hhid_task(void)
{
    usb_hhid_task();
}   /* eof R_usb_hhid_task() */


/******************************************************************************
Function Name   : usb_hhid_class_check
Description     : Check connected device
Arguments       : uint16_t  **table
Return value    : none
******************************************************************************/
void R_usb_hhid_ClassCheck(uint8_t **table)
{
    usb_shhid_device_table      = (uint8_t*)((uint16_t*)table[0]);  /* Device Descriptor Table */
    usb_ghhid_ConfigTable       = (uint8_t*)((uint16_t*)table[1]);  /* Configuration Descriptor Table */
    usb_ghhid_Devaddr           = (usb_addr_t)(*table[2]);          /* Device Address */

    /* Enumeration Sequence String Descriptor #0 receive request */
    usb_shhid_enum_seq  = (uint16_t)USB_HHID_ENUM_STR_DT0_REQ;
    R_USB_GET_SND(USB_HHID_MBX, USB_HHID_TCMD_OPEN, &usb_hhid_dummy_function, USB_NULL);
}   /* eof usb_hhid_class_check() */


/******************************************************************************
Function Name   : R_usb_hhid_DriverStart
Description     : Start of HID driver
Arguments       : none
Return value    : none
******************************************************************************/
void R_usb_hhid_DriverStart(void)
{
    usb_hhid_initialize();
}   /* eof R_usb_hhid_DriverStart() */


/******************************************************************************
Function Name   : R_usb_hhid_DriverStop
Description     : Stop of HID driver
Arguments       : none
Return value    : none
******************************************************************************/
void R_usb_hhid_DriverStop(void)
{
    /* Endpoint Table initilize */
    usb_ghhid_SmplEpTbl[1]  = (uint16_t)( USB_NULL | USB_BFREOFF | USB_NULL
                              | USB_NULL | USB_SHTNAKOFF | USB_NULL | USB_NULL);
    usb_ghhid_SmplEpTbl[2]  = (uint16_t)(USB_NULL);
    usb_ghhid_SmplEpTbl[3]  = (uint16_t)(USB_NULL);
}   /* eof R_usb_hhid_DriverStop() */


/******************************************************************************
Function Name   : R_usb_hhid_SetPipeRegistration
Description     : Host HID Pipe registration
Arguments       : usb_addr_t  devadr        : Device address
Return value    : none
******************************************************************************/
void R_usb_hhid_SetPipeRegistration(usb_addr_t devadr)
{
//@@@    uint8_t     *table;

//@@@    /* Get Configuration descriptor Address */
//@@@    table   = usb_ghhid_interface_table;

    /* Loop for bNumEndpoints - Configuration Descriptor */
    if (devadr != 0)
    {
        /* Device Address Set for Endpoint Table */
//@@@        usb_ghhid_SmplEpTbl[2 + (0*USB_EPL)] |= USB_ADDR2DEVSEL(devadr);
        usb_ghhid_SmplEpTbl[2] |= USB_ADDR2DEVSEL(devadr);
    }
    /* Set pipe configuration request(Interrupt IN) */
    R_usb_hstd_SetPipeRegistration((uint16_t*)&usb_ghhid_SmplEpTbl[0], USB_YES);
}   /* eof R_usb_hhid_SetPipeRegistration() */


/******************************************************************************
Function Name   : R_usb_hhid_PipeTransferExample
Description     : Start of data transfer
Arguments       : uint8_t *table            : data address
                : uint32_t size             : data Size
                : usb_cb_t complete         : callback function
Return value    : none
******************************************************************************/
usb_er_t R_usb_hhid_PipeTransferExample(uint8_t *table, usb_leng_t size, usb_cb_t complete)
{
    usb_shhid_transfer_message.tranadr    = table;
    usb_shhid_transfer_message.tranlen    = size;
    usb_shhid_transfer_message.setup        = 0;
    return R_USB_GET_SND(USB_HHID_MBX, USB_HHID_TCMD_DATA_TRANS, complete, USB_HHID_USE_PIPE);
}   /* eof R_usb_hhid_PipeTransferExample() */


/******************************************************************************
Function Name   : R_usb_hhid_TransferEnd
Description     : End of data transfer
Arguments       : none
Return value    : usb_er_t                  : USB_E_OK etc
******************************************************************************/
usb_er_t R_usb_hhid_TransferEnd(void)
{
    usb_er_t    err;

#ifdef USB_HHID_GET_REPORT_PIPE0
    /* End of data transfer(USB1-IP host function) */
    err = R_usb_hstd_TransferEnd(USB_PIPE0, USB_DO_TRANSFER_STP);
#else   /* USB_HHID_GET_REPORT_PIPE0 is not defined */
    /* End of data transfer(USB1-IP host function) */
    err = R_usb_hstd_TransferEnd(USB_HHID_USE_PIPE, USB_DO_TRANSFER_STP);
#endif  /* USB_HHID_GET_REPORT_PIPE0 */

    return err;
}   /* eof R_usb_hhid_TransferEnd() */


/******************************************************************************
Function Name   : R_usb_hhid_class_request
Description     : class request message Send for Host HID task
Arguments       : USB_HHID_CLASS_REQUEST_PARM_t *pram   : class request Parametor
Return value    : usb_er_t                              : Error info
******************************************************************************/
usb_er_t R_usb_hhid_class_request(USB_HHID_CLASS_REQUEST_PARM_t *pram)
{

    /* Class request Parametor Set */
    usb_ghhid_class_request_parm = *(USB_HHID_CLASS_REQUEST_PARM_t*)pram;

    /* Get mem block from pool. */
    return R_USB_GET_SND(USB_HHID_MBX, USB_HHID_TCMD_CLASS_REQ, &usb_hhid_dummy_function, (usb_strct_t)pram->devadr);

}   /* eof R_usb_hhid_class_request() */


/******************************************************************************
Function Name   : R_usb_hhid_DeviceInformation
Description     : Get Device Information
Arguments       : uint16_t  *deviceinfo   : Table Pointer
Return value    : none
******************************************************************************/
void R_usb_hhid_DeviceInformation(uint16_t *deviceinfo)
{
    /* Return to device information */
    R_usb_hstd_DeviceInformation(usb_ghhid_Devaddr, deviceinfo);
}   /* eof R_usb_hhid_DeviceInformation() */


/******************************************************************************
Function Name   : R_usb_hhid_ChangeDeviceState
Description     : Host HID Device State Change
Arguments       : uint16_t   msginfo        : Message Info
                : usb_port_t   port         : Port number
                : usb_cbinfo_t callback     : Callback function
Return value    : none
******************************************************************************/
void R_usb_hhid_ChangeDeviceState(uint16_t msginfo, usb_port_t port, usb_cbinfo_t callback)
{
    /* Change HUB down port device status */
    R_usb_hstd_MgrChangeDeviceState(callback, (usb_strct_t)msginfo, (usb_strct_t)port);
}   /* eof R_usb_hhid_ChangeDeviceState() */


/******************************************************************************
Function Name   : R_usb_hhid_GetReportLength
Description     : Get Report Length
Arguments       : none
Return value    : uint16_t                  : Report Length
******************************************************************************/
uint16_t R_usb_hhid_GetReportLength(void)
{
    /* Max Packet Size */
    return usb_ghhid_maxps;
}   /* eof R_usb_hhid_GetReportLength() */


/******************************************************************************
Function Name   : R_usb_hhid_get_hid_protocol
Description     : Get HID Protocol
Arguments       : none
Return value    : Hid Protocol
******************************************************************************/
uint8_t R_usb_hhid_get_hid_protocol(void)
{
    /* table[USB_HID_IFPRO_NUM]:Interface Descriptor-bInterfaceProtocol */
    return usb_ghhid_interface_table[USB_HID_IFPRO_NUM];
}   /* eof R_usb_hhid_get_hid_protocol() */


/******************************************************************************
End Of File
******************************************************************************/
