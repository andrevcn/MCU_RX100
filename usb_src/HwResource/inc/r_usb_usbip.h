/******************************************************************************
* File Name    : r_usb_usbip.h
* Version      : 2.10
* Description  : USB-IP define
*******************************************************************************
* History   : DD.MM.YYYY Version Description
*           : 01.08.2013 2.10    Basic Mini FW Ver2.10 Release
******************************************************************************/
#ifndef __R_USB_USBIP_H__
#define __R_USB_USBIP_H__

/******************************************************************************
Includes <System Includes> , "Project Includes"
******************************************************************************/
//#include "iodefine.h"             /* Type define */

/*****************************************************************************
USB_597IP_PP Register definition(not change)
******************************************************************************/
/* System Configuration Control Register */
/*  USB_SYSCFG0     (*((REGP*)(USB_BASE+0x00))) */
#define USB_SCKE                (0x0400u)   /* b10: USB clock enable */
#define USB_CNEN                (0x0100u)   /* b8: To copy from the english DS for BC.  */
#define USB_DCFM                (0x0040u)   /* b6: Function select */
#define USB_DRPD                (0x0020u)   /* b5: D+/D- pull down control */
#define USB_DPRPU               (0x0010u)   /* b4: D+ pull up control */
#define USB_USBE                (0x0001u)   /* b0: USB module enable */

/* System Configuration Status Register */
/*  USB_SYSSTS0     (*((REGP*)(USB_BASE+0x04))) */
#define USB_OVCBIT              (0x8000u)   /* b15-14: Over-current bit */
#define USB_HTACT               (0x0040u)   /* b6: Sequencer status monitor */
#define USB_IDMON               (0x0004u)   /* b2: ID-pin monitor */
#define USB_LNST                (0x0003u)   /* b1-0: D+, D- line status */
#define   USB_SE1               (0x0003u)       /* SE1 */
#define   USB_FS_KSTS           (0x0002u)       /* Full-Speed K State */
#define   USB_FS_JSTS           (0x0001u)       /* Full-Speed J State */
#define   USB_LS_JSTS           (0x0002u)       /* Low-Speed J State */
#define   USB_LS_KSTS           (0x0001u)       /* Low-Speed K State */
#define   USB_SE0               (0x0000u)       /* SE0 */

/* Device State Control Register */
/*  USB_DVSTCTR0        (*((REGP*)(USB_BASE+0x08))) */
#define USB_HNPBTOA             (0x0800u)   /* b11: Host negotiation protocol (BtoA) */
#define USB_VBOUT               (0x0200u)   /* b9: VBUS output */
#define USB_WKUP                (0x0100u)   /* b8: Remote wakeup */
#define USB_RWUPE               (0x0080u)   /* b7: Remote wakeup sense */
#define USB_USBRST              (0x0040u)   /* b6: USB reset enable */
#define USB_RESUME              (0x0020u)   /* b5: Resume enable */
#define USB_UACT                (0x0010u)   /* b4: USB bus enable */
#define USB_RHST                (0x0007u)   /* b2-0: Reset handshake status */
#define   USB_HSPROC            (0x0004u)       /* HS handshake processing */
#define   USB_HSMODE            (0x0003u)       /* Hi-Speed mode */
#define   USB_FSMODE            (0x0002u)       /* Full-Speed mode */
#define   USB_LSMODE            (0x0001u)       /* Low-Speed mode */
#define   USB_UNDECID           (0x0000u)       /* Undecided */

/* CFIFO Port Register */
/*  USB_CFIFO           (*((REGP*)(USB_BASE+0x14))) */
/*  USB_CFIFO_8         (*((REGP8*)(USB_BASE+0x14))) */

/* CFIFO Port Select Register */
/*  USB_CFIFOSEL        (*((REGP*)(USB_BASE+0x20))) */
#define USB_RCNT                (0x8000u)   /* b15: Read count mode */
#define USB_REW                 (0x4000u)   /* b14: Buffer rewind */

#ifdef __RX
#define USB_DCLRM               (0x2000u)   /* b13: DMA buffer clear mode */
#define USB_DREQE               (0x1000u)   /* b12: DREQ output enable */
#endif  /* __RX */

#define USB_MBW                 (0x0400u)   /* b10: Maximum bit width for FIFO access */
#define   USB_MBW_8             (0x0000u)       /*  8bit */
#define   USB_MBW_16            (0x0400u)       /* 16bit */
#define USB_BIGEND              (0x0100u)   /* b8: Big endian mode */
#define   USB_FIFO_LITTLE       (0x0000u)       /* Little dendian */
#define   USB_FIFO_BIG          (0x0100u)       /* Big endifan */
#define USB_ISEL                (0x0020u)   /* b5: DCP FIFO port direction select */
#define USB_CURPIPE             (0x000Fu)   /* b3-0: PIPE select */

/* CFIFO Port Control Register */
/*  USB_CFIFOCTR        (*((REGP*)(USB_BASE+0x22))) */
#define USB_BVAL                (0x8000u)   /* b15: Buffer valid flag */
#define USB_BCLR                (0x4000u)   /* b14: Buffer clear */
#define USB_FRDY                (0x2000u)   /* b13: FIFO ready */
#define USB_DTLN                (0x01FFu)   /* b8-0: FIFO data length */


/* Interrupt Enable Register 0 */
/*  USB_INTENB0     (*((REGP*)(USB_BASE+0x30))) */
#define USB_VBSE                (0x8000u)   /* b15: VBUS interrupt */
#define USB_RSME                (0x4000u)   /* b14: Resume interrupt */
#define USB_SOFE                (0x2000u)   /* b13: Frame update interrupt */
#define USB_DVSE                (0x1000u)   /* b12: Device state transition interrupt */
#define USB_CTRE                (0x0800u)   /* b11: Control transfer stage transition interrupt */
#define USB_BEMPE               (0x0400u)   /* b10: Buffer empty interrupt */
#define USB_NRDYE               (0x0200u)   /* b9: Buffer notready interrupt */
#define USB_BRDYE               (0x0100u)   /* b8: Buffer ready interrupt */

/* Interrupt Enable Register 1 */
/*  USB_INTENB1     (*((REGP*)(USB_BASE+0x32))) */
#define USB_OVRCRE              (0x8000u)   /* b15: Over-current interrupt */
#define USB_BCHGE               (0x4000u)   /* b14: USB us chenge interrupt */
#define USB_DTCHE               (0x1000u)   /* b12: Detach sense interrupt */
#define USB_ATTCHE              (0x0800u)   /* b11: Attach sense interrupt */
#define USB_EOFERRE             (0x0040u)   /* b6: EOF error interrupt */
#define USB_SIGNE               (0x0020u)   /* b5: SETUP IGNORE interrupt */
#define USB_SACKE               (0x0010u)   /* b4: SETUP ACK interrupt */
#define USB_PDDETINTE           (0x0001u)   /* b0: SETUP PDDETINT interrupt */

/* BRDY Interrupt Enable/Status Register */
/*  USB_BRDYENB     (*((REGP*)(USB_BASE+0x36))) */
/*  USB_BRDYSTS     (*((REGP*)(USB_BASE+0x46))) */
#define USB_BRDY7               (0x0080u)   /* b7: PIPE7 */
#define USB_BRDY6               (0x0040u)   /* b6: PIPE6 */
#define USB_BRDY5               (0x0020u)   /* b5: PIPE5 */
#define USB_BRDY4               (0x0010u)   /* b4: PIPE4 */
#define USB_BRDY0               (0x0001u)   /* b0: PIPE0 */

/* NRDY Interrupt Enable/Status Register */
/*  USB_NRDYENB     (*((REGP*)(USB_BASE+0x38))) */
/*  USB_NRDYSTS     (*((REGP*)(USB_BASE+0x48))) */
#define USB_NRDY7               (0x0080u)   /* b7: PIPE7 */
#define USB_NRDY6               (0x0040u)   /* b6: PIPE6 */
#define USB_NRDY5               (0x0020u)   /* b5: PIPE5 */
#define USB_NRDY4               (0x0010u)   /* b4: PIPE4 */
#define USB_NRDY0               (0x0001u)   /* b0: PIPE0 */

/* BEMP Interrupt Enable/Status Register */
/*  USB_BEMPENB     (*((REGP*)(USB_BASE+0x3A))) */
/*  USB_BEMPSTS     (*((REGP*)(USB_BASE+0x4A))) */
#define USB_BEMP7               (0x0080u)   /* b7: PIPE7 */
#define USB_BEMP6               (0x0040u)   /* b6: PIPE6 */
#define USB_BEMP5               (0x0020u)   /* b5: PIPE5 */
#define USB_BEMP4               (0x0010u)   /* b4: PIPE4 */
#define USB_BEMP0               (0x0001u)   /* b0: PIPE0 */

/* SOF Pin Configuration Register */
/*  USB_SOFCFG          (*((REGP*)(USB_BASE+0x3C))) */
#define USB_BRDYM               (0x0040u)   /* b6: BRDY clear timing */

/* Interrupt Status Register 0 */
/*  USB_INTSTS0     (*((REGP*)(USB_BASE+0x40))) */
#define USB_VBINT               (0x8000u)   /* b15: VBUS interrupt */
#define USB_RESM                (0x4000u)   /* b14: Resume interrupt */
#define USB_SOFR                (0x2000u)   /* b13: SOF update interrupt */
#define USB_DVST                (0x1000u)   /* b12: Device state transition interrupt */
#define USB_CTRT                (0x0800u)   /* b11: Control transfer stage transition interrupt */
#define USB_BEMP                (0x0400u)   /* b10: Buffer empty interrupt */
#define USB_NRDY                (0x0200u)   /* b9: Buffer notready interrupt */
#define USB_BRDY                (0x0100u)   /* b8: Buffer ready interrupt */
#define USB_VBSTS               (0x0080u)   /* b7: VBUS input port */
#define USB_DVSQ                (0x0070u)   /* b6-4: Device state */
#define   USB_DS_SPD_CNFG       (0x0070u)       /* Suspend Configured */
#define   USB_DS_SPD_ADDR       (0x0060u)       /* Suspend Address */
#define   USB_DS_SPD_DFLT       (0x0050u)       /* Suspend Default */
#define   USB_DS_SPD_POWR       (0x0040u)       /* Suspend Powered */
#define   USB_DS_SUSP           (0x0040u)       /* Suspend */
#define   USB_DS_CNFG           (0x0030u)       /* Configured */
#define   USB_DS_ADDS           (0x0020u)       /* Address */
#define   USB_DS_DFLT           (0x0010u)       /* Default */
#define   USB_DS_POWR           (0x0000u)       /* Powered */
#define USB_DVSQS               (0x0030u)   /* b5-4: Device state */
#define USB_VALID               (0x0008u)   /* b3: Setup packet detect flag */
#define USB_CTSQ                (0x0007u)   /* b2-0: Control transfer stage */
#define   USB_CS_SQER           (0x0006u)       /* Sequence error */
#define   USB_CS_WRND           (0x0005u)       /* Ctrl write nodata status stage */
#define   USB_CS_WRSS           (0x0004u)       /* Ctrl write status stage */
#define   USB_CS_WRDS           (0x0003u)       /* Ctrl write data stage */
#define   USB_CS_RDSS           (0x0002u)       /* Ctrl read status stage */
#define   USB_CS_RDDS           (0x0001u)       /* Ctrl read data stage */
#define   USB_CS_IDST           (0x0000u)       /* Idle or setup stage */

/* Interrupt Status Register 1 */
/*  USB_INTSTS1     (*((REGP*)(USB_BASE+0x42))) */
#define USB_OVRCR               (0x8000u)   /* b15: Over-current interrupt */
#define USB_BCHG                (0x4000u)   /* b14: USB bus chenge interrupt */
#define USB_DTCH                (0x1000u)   /* b12: Detach sense interrupt */
#define USB_ATTCH               (0x0800u)   /* b11: Attach sense interrupt */
#define USB_EOFERR              (0x0040u)   /* b6: EOF-error interrupt */
#define USB_SIGN                (0x0020u)   /* b5: Setup ignore interrupt */
#define USB_SACK                (0x0010u)   /* b4: Setup ack interrupt */
#define USB_PDDETINT            (0x0001u)   /* b0: Pddetint sense interrupt */

/* Frame Number Register */
/*  USB_FRMNUM          (*((REGP*)(USB_BASE+0x4C))) */
#define USB_FRNM                (0x07FFu)   /* b10-0: Frame number */

/* USB Address / Low Power Status Recovery Register */
/*  USB_USBADDR     (*((REGP*)(USB_BASE+0x50))) */
#define USB_USBADDR_MASK        (0x007Fu)   /* b6-0: USB address */

/* USB Request Type Register */
/*  USB_USBREQ          (*((REGP*)(USB_BASE+0x54))) */
#define USB_BREQUEST            (0xFF00u)   /* b15-8: USB_BREQUEST */
#define USB_BMREQUESTTYPE       (0x00FFu)   /* b7-0: USB_BMREQUESTTYPE */
#define USB_BMREQUESTTYPEDIR    (0x0080u)   /* b7  : Data transfer direction */
#define USB_BMREQUESTTYPETYPE   (0x0060u)   /* b6-5: Type */
#define USB_BMREQUESTTYPERECIP  (0x001Fu)   /* b4-0: Recipient */

/* USB Request Value Register */
/*  USB_USBVAL          (*((REGP*)(USB_BASE+0x56))) */
#define USB_WVALUE              (0xFFFFu)   /* b15-0: wValue */
#define USB_DT_TYPE             (0xFF00u)
#define USB_GET_DT_TYPE(v)      (((v) & USB_DT_TYPE) >> 8)
#define USB_DT_INDEX            (0x00FFu)
#define USB_CONF_NUM            (0x00FFu)
#define USB_ALT_SET             (0x00FFu)

/* USB Request Index Register */
/*  USB_USBINDX     (*((REGP*)(USB_BASE+0x58))) */
#define USB_WINDEX              (0xFFFFu)   /* b15-0: wIndex */
#define USB_TEST_SELECT         (0xFF00u)   /* b15-b8: Test Mode Selectors */
#define   USB_TEST_J            (0x0100u)       /* Test_J */
#define   USB_TEST_K            (0x0200u)       /* Test_K */
#define   USB_TEST_SE0_NAK      (0x0300u)       /* Test_SE0_NAK */
#define   USB_TEST_PACKET       (0x0400u)       /* Test_Packet */
#define   USB_TEST_FORCE_ENABLE (0x0500u)       /* Test_Force_Enable */
#define   USB_TEST_STSelectors  (0x0600u)       /* Standard test selectors */
#define   USB_TEST_RESERVED     (0x4000u)       /* Reserved */
#define   USB_TEST_VSTMODES     (0xC000u)       /* VendorSpecific test modes */
#define USB_EP_DIR              (0x0080u)   /* b7: Endpoint Direction */
#define   USB_EP_DIR_IN         (0x0080u)
#define   USB_EP_DIR_OUT        (0x0000u)

/* USB Request Length Register */
/*  USB_USBLENG     (*((REGP*)(USB_BASE+0x5A))) */
#define USB_WLENGTH             (0xFFFFu)   /* b15-0: wLength */


/* Pipe Window Select Register */
/*  USB_PIPESEL     (*((REGP*)(USB_BASE+0x64))) */
#define USB_PIPENM              (0x000Fu)   /* b3-0: Pipe select */

/* Default Control Pipe Configuration Register */
/*  USB_DCPCFG          (*((REGP*)(USB_BASE+0x5C))) */
/* Pipe Configuration Register */
/*  USB_PIPECFG     (*((REGP*)(USB_BASE+0x68))) */
/* Refer to usbd_DefUSBIP.h */
#define USB_TYPE            (0xC000u)   /* b15-14: Transfer type */
#define USB_BFRE            (0x0400u)   /* b10: Buffer ready interrupt mode select */
#define USB_DBLB            (0x0200u)   /* b9: Double buffer mode select */
#define USB_SHTNAK          (0x0080u)   /* b7: Transfer end NAK */
#define USB_DIR             (0x0010u)   /* b4: Transfer direction select */
#define USB_EPNUM           (0x000Fu)   /* b3-0: Eendpoint number select */

/* Default Control Pipe Maxpacket Size Register */
/* Pipe Maxpacket Size Register */
/*  USB_DCPMAXP     (*((REGP*)(USB_BASE+0x5E))) */
/* Pipe Maxpacket Size Register */
/*  USB_PIPEMAXP        (*((REGP*)(USB_BASE+0x6C))) */
#define USB_DEVSEL              (0xF000u)   /* b15-12: Device address select */
#define USB_MAXP                (0x007Fu)   /* b6-0: Maxpacket size of default control pipe */
#define USB_MXPS                (0x01FFu)   /* b8-0: Maxpacket size */

/* Pipe Cycle Configuration Register */
/*  USB_PIPEPERI        (*((REGP*)(USB_BASE+0x6E))) */
/* Refer to usbd_DefUSBIP.h */
#define USB_IITV            (0x0007u)   /* b2-0: Isochronous interval */

/* Default Control Pipe Control Register */
/*  USB_DCPCTR          (*((REGP*)(USB_BASE+0x60))) */
/* Pipex Control Register */
/*  USB_PIPE4CTR        (*((REGP*)(USB_BASE+0x76))) */
/*  USB_PIPE5CTR        (*((REGP*)(USB_BASE+0x78))) */
/*  USB_PIPE6CTR        (*((REGP*)(USB_BASE+0x7A))) */
/*  USB_PIPE7CTR        (*((REGP*)(USB_BASE+0x7C))) */
#define USB_BSTS                (0x8000u)   /* b15: Buffer status */
#define USB_SUREQ               (0x4000u)   /* b14: Send USB request  */
#define USB_INBUFM              (0x4000u)   /* b14: IN buffer monitor (Only for PIPE1 to 5) */
#define USB_SUREQCLR            (0x0800u)   /* b11: stop setup request */
#define USB_ATREPM              (0x0400u)   /* b10: Auto repeat mode */
#define USB_ACLRM               (0x0200u)   /* b9: buffer auto clear mode */
#define USB_SQCLR               (0x0100u)   /* b8: Sequence bit clear */
#define USB_SQSET               (0x0080u)   /* b7: Sequence bit set */
#define USB_SQMON               (0x0040u)   /* b6: Sequence bit monitor */
#define USB_PBUSY               (0x0020u)   /* b5: pipe busy */
#define USB_CCPL                (0x0004u)   /* b2: Enable control transfer complete */
#define USB_PID                 (0x0003u)   /* b1-0: Response PID */
#define   USB_PID_STALL         (0x0002u)       /* STALL */
#define   USB_PID_BUF           (0x0001u)       /* BUF */
#define   USB_PID_NAK           (0x0000u)       /* NAK */


/* PIPExTRE */
/*  USB_PIPE4TRE        (*((REGP*)(USB_BASE+0x9C))) */
/*  USB_PIPE5TRE        (*((REGP*)(USB_BASE+0xA0))) */
#define USB_TRENB               (0x0200u)   /* b9: Transaction count enable */
#define USB_TRCLR               (0x0100u)   /* b8: Transaction count clear */


/* PIPExTRN */
/*  USB_PIPE4TRN        (*((REGP*)(USB_BASE+0x9E))) */
/*  USB_PIPE5TRN        (*((REGP*)(USB_BASE+0xA2))) */
#define USB_TRNCNT              (0xFFFFu)   /* b15-0: Transaction counter */

/* USBBCCTRLx */
#define USB_PDDETSTS            (0x0200u)
#define USB_CHGDETSTS           (0x0100u)
#define USB_BATCHGE             (0x0080u)
#define USB_DCPMODE             (0x0040u)
#define USB_VDMSRCE             (0x0020u)
#define USB_IDPSINKE            (0x0010u)
#define USB_VDPSRCE             (0x0008u)
#define USB_IDMSINKE            (0x0004u)
#define USB_IDPSRCE             (0x0002u)
#define USB_RPDME               (0x0001u)

/* DEVADDx */
/*  USB_DEVADD0     (*((REGP*)(USB_BASE+0xD0))) */
/*  USB_DEVADD1     (*((REGP*)(USB_BASE+0xD2))) */
/*  USB_DEVADD2     (*((REGP*)(USB_BASE+0xD4))) */
/*  USB_DEVADD3     (*((REGP*)(USB_BASE+0xD6))) */
/*  USB_DEVADD4     (*((REGP*)(USB_BASE+0xD8))) */
/*  USB_DEVADD5     (*((REGP*)(USB_BASE+0xDA))) */
#define USB_USBSPD              (0x00C0u)
#define   USB_NOTUSED           (0x0000u)
#define   USB_FULLSPEED         (0x0080u)       /* Full-Speed connect */
#define   USB_HIGHSPEED         (0x00C0u)       /* Hi-Speed connect */
#define   USB_LOWSPEED          (0x0040u)       /* Low-Speed connect */


/* The bit definition that doesn't exist in R8C for the FW sharing. */
#define USB_DMRPU               (0x0008u)   /* USB_SYSCFG0 b3: D- pull up control */
#define USB_TRNENSEL            (0x0100u)   /* USB_SOFCFG  b8: Select transaction enable period */
#define USB_RTPORT              (0x0001u)   /* DEVADDx     b0: Route port number */

#endif  /* __R_USB_USBIP_H__ */
/******************************************************************************
End of file
******************************************************************************/
