/*******************************************************************************
* File Name    : r_usb_regaccess.h
* Version      : 2.10
* Description  : USB Basic firmware library function prototype declaration
*******************************************************************************/
/*******************************************************************************
* History   : DD.MM.YYYY Version Description
*           : 01.08.2013 2.10    Basic Mini FW Ver2.10 Release
*******************************************************************************/
#ifndef __R_USB_REGACCESS_H__
#define __R_USB_REGACCESS_H__

#include "r_usb_ctypedef.h"

/*****************************************************************************
USB register access macro definitions
******************************************************************************/
/*------------------------------------------------------------
 * USB register access macro (Basic)
 *------------------------------------------------------------*/
/* Read/Write USB register */

#define USB_RD( r, v )      (v  = USB_IP.r.WORD)
#define USB_WR(r,v)             (USB_IP.r.WORD=v)
  
#define  USB_RDW( r, v )         (( v ) = ( USB_IP.r ))
#define  USB_WRW( r, v )         (( USB_IP.r ) = ( v ))


/*------------------------------------------------------------
 * USB register bit access macro
 *------------------------------------------------------------*/
/* Set bit(s) of USB register    */
/* r : USB register              */
/* v : Value to set              */
#define  USB_SET_PAT( r, v ) (( USB_IP.r.WORD ) |= ( v ))


/* Reset bit(s) of USB register  */
/* r : USB register              */
/* m : Bit pattern to reset      */
#define  USB_CLR_PAT( r, m ) (( USB_IP.r.WORD ) &= ( (uint16_t)(~(m)) ))


/* modify bit(s) of USB register */
/* r : USB register              */
/* v : Value to set              */
/* m : Bit pattern to modify     */
#define  USB_MDF_PAT( r, v, m )                                                 \
                                    USB_RD( r, g_usb_LibTemp );             \
                                    g_usb_LibTemp &= ( (uint16_t)(~(m)) );      \
                                    g_usb_LibTemp |= ( (uint16_t)(v & m) ); \
                                    USB_WR( r, g_usb_LibTemp )                  \

/* Reset bit(s) of USB status    */
/* r : USB register              */
/* m : Bit pattern to reset      */
#define  USB_CLR_STS( r, m )    USB_WR( r, (uint16_t)(~m))

/* Set bit(s) of USB status      */
/* r : USB register              */
/* m : Dummy                     */
#define  USB_SET_STS( r, m )    USB_WR( r, 0xFFFFu )

//@@@   It will correct it later because there is xxx in the number. 

#endif  /* __R_USB_REGACCESS_H__ */
/******************************************************************************
End of file
******************************************************************************/
