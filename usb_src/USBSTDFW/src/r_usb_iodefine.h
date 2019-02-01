/*******************************************************************************
* File Name    : r_usb_iodefine.h
* Version      : 2.10
* Description  : USB Basic firmware library function prototype declaration
*******************************************************************************/
/*******************************************************************************
* History   : DD.MM.YYYY Version Description
*           : 31.03.2012 1.80    Basic FW mini Ver2.00(ES Version)
*           : 30.11.2012 2.00
*           : 01.08.2013 2.10    Basic Mini FW Ver2.10 Release
*******************************************************************************/
#ifndef __R_USB_IODEFINE_H__
#define __R_USB_IODEFINE_H__

/*****************************************************************************
USB register access macro definitions
******************************************************************************/
/*------------------------------------------------------------
 * USB register access macro (Basic)
 *------------------------------------------------------------*/
/* Read/Write USB register */
#define  USB_RD( r, v )         (( v ) = ( USB_IP.r.WORD ))
#define  USB_WR( r, v )         (( USB_IP.r.WORD ) = ( v ))

/*------------------------------------------------------------
 * USB register bit access macro
 *------------------------------------------------------------*/
/* Set bit(s) of USB register    */
/* r : USB register              */
/* v : Value to set              */
#define  USB_SET_PAT( r, v ) \
    (( USB_IP.r.WORD ) |= ( v ))


/* Reset bit(s) of USB register  */
/* r : USB register              */
/* m : Bit pattern to reset      */
#define  USB_CLR_PAT( r, m ) \
    (( USB_IP.r.WORD ) &= ( (uint16_t)(~(m)) ))


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
#define  USB_CLR_STS( r, m )    USB_WR( r, ( (uint16_t)(~(m)) ) )

/* Set bit(s) of USB status      */
/* r : USB register              */
/* m : Dummy                     */
#define  USB_SET_STS( r, m )    USB_WR( r, 0xFFFFu )

//@@@   It will correct it later because there is xxx in the number. 
struct st_usbip {
    union {
        unsigned short WORD;
        struct {
            unsigned short USBE:1;
            unsigned short :3;
            unsigned short DPRPU:1;
            unsigned short DRPD:1;
            unsigned short DCFM:1;
            unsigned short :3;
            unsigned short SCKE:1;
            unsigned short :5;
        } BIT;
    } SYSCFG;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0001;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short :15;
            unsigned short BERRS:1;
        } BIT;
    } SYSCFG1;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0002;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short LIST:2;
            unsigned short IDMON:1;
            unsigned short :3;
            unsigned short HTACT:1;
            unsigned short :7;
            unsigned short OCVMON:2;
        } BIT;
    } SYSSTS0;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0003;
#endif
    union {
        unsigned short WORD;
    } SYSSTS1;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0004;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short RHST:3;
            unsigned short :1;
            unsigned short UACT:1;
            unsigned short RESUME:1;
            unsigned short USBRST:1;
            unsigned short RWUPE:1;
            unsigned short WKUP:1;
            unsigned short VBOUT:1;
            unsigned short EXTLP0:1;
            unsigned short HNPBTOA:1;
            unsigned short :4;
        } BIT;
    } DVSTCTR0;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0005;
#endif
    union {
        unsigned short WORD;
    } DVSTCTR1;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0006;
#endif
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_1;  /* = TESTMODE */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0007;
#endif
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_2;  /* = PINCFG */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0008;
#endif
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_3;  /* = DMA0CFG */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0009;
#endif
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_4;  /* = DMA1CFG */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No000A;
#endif
    union {
        unsigned short WORD;
        struct {
#ifdef __BIG
            /* Big-endian */
            unsigned char H;
            unsigned char L;
#else
            /* Little-endian */
            unsigned char L;
            unsigned char H;
#endif
        } BYTE;
    } CFIFO;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No000B;
#endif
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_5;  /* = Not assignment */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No000C;
#endif
    union {
        unsigned short WORD;
        struct {
#ifdef __BIG
            /* Big-endian */
            unsigned char H;
            unsigned char L;
#else
            unsigned char L;
            unsigned char H;
#endif
        } BYTE;
    } D0FIFO;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No000D;
#endif
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_6;  /* = Not assignment */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No000E;
#endif
    union {
        unsigned short WORD;
        struct {
#ifdef __BIG
            /* Big-endian */
            unsigned char H;
            unsigned char L;
#else
            unsigned char L;
            unsigned char H;
#endif
        } BYTE;
    } D1FIFO;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No000F;
#endif
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_7;  /* = Not assignment */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0010;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short CURPIPE:4;
            unsigned short :1;
            unsigned short ISEL:1;
            unsigned short :2;
            unsigned short BIGEND:1;
            unsigned short :1;
            unsigned short MBW:1;
            unsigned short :3;
            unsigned short REW:1;
            unsigned short RCNT:1;
        } BIT;
    } CFIFOSEL;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0011;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short DTLN:9;
            unsigned short :4;
            unsigned short FRDY:1;
            unsigned short BCLR:1;
            unsigned short BVAL:1;
        } BIT;
    } CFIFOCTR;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0012;
#endif
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_8;  /* = Not assignment */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0013;
#endif
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_9;  /* = Not assignment */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0014;
#endif
    union {
        unsigned short WORD;
#ifdef __RX
        struct {
            unsigned short RCNT:1;
            unsigned short REW:1;
            unsigned short DCLRM:1;
            unsigned short DREQE:1;
            unsigned short :1;
            unsigned short MBW:1;
            unsigned short :1;
            unsigned short BIGEND:1;
            unsigned short :4;
            unsigned short CURPIPE:4;
        } BIT;
#endif  /* __RX */
    } D0FIFOSEL;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0015;
#endif
    union {
        unsigned short WORD;
#ifdef __RX
        struct {
            unsigned short BVAL:1;
            unsigned short BCLR:1;
            unsigned short FRDY:1;
            unsigned short :4;
            unsigned short DTLN:9;
        } BIT;
#endif  /* __RX */
    } D0FIFOCTR;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0016;
#endif
    union {
        unsigned short WORD;
    } D1FIFOSEL;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0017;
#endif
    union {
        unsigned short WORD;
    } D1FIFOCTR;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0018;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short :8;
            unsigned short BRDYE:1;
            unsigned short NRDYE:1;
            unsigned short BEMPE:1;
            unsigned short CTRE:1;
            unsigned short DVSE:1;
            unsigned short SOFE:1;
            unsigned short RSME:1;
            unsigned short VBSE:1;
        } BIT;
    } INTENB0;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0019;
#endif
    union{
        unsigned short WORD;
        struct{
            unsigned short PDDETINTE:1;
            unsigned short :3;
            unsigned short SACKE:1;
            unsigned short SIGNE:1;
            unsigned short EOFERRE:1;
            unsigned short :4;
            unsigned short ATTCHE:1;
            unsigned short DTCHE:1;
            unsigned short :1;
            unsigned short BCHGE:1;
            unsigned short OVRCRE:1;
        }BIT;
    }INTENB1;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No001A;
#endif
    union {
        unsigned short WORD;
        struct{
            unsigned short PDDETINTE:1;
            unsigned short :5;
            unsigned short EOFERRE:1;
            unsigned short :4;
            unsigned short ATTCHE:1;
            unsigned short DTCHE:1;
            unsigned short :1;
            unsigned short BCHGE:1;
            unsigned short OVRCRE:1;
        }BIT;
    } INTENB2;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No001B;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short PIPE0BRDYE:1;
            unsigned short :3;
            unsigned short PIPE4BRDYE:1;
            unsigned short PIPE5BRDYE:1;
            unsigned short PIPE6BRDYE:1;
            unsigned short PIPE7BRDYE:1;
            unsigned short :8;
        } BIT;
    } BRDYENB;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No001C;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short PIPE0NRDYE:1;
            unsigned short :3;
            unsigned short PIPE4NRDYE:1;
            unsigned short PIPE5NRDYE:1;
            unsigned short PIPE6NRDYE:1;
            unsigned short PIPE7NRDYE:1;
            unsigned short :8;
        } BIT;
    } NRDYENB;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No001D;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short PIPE0BEMPE:1;
            unsigned short :3;
            unsigned short PIPE4BEMPE:1;
            unsigned short PIPE5BEMPE:1;
            unsigned short PIPE6BEMPE:1;
            unsigned short PIPE7BEMPE:1;
            unsigned short :8;
        } BIT;
    } BEMPENB;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No001E;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short :4;
            unsigned short EDGESTS:1;
            unsigned short :1;
            unsigned short BRDYM:1;
            unsigned short :9;
        } BIT;
    } SOFCFG;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No001F;
#endif
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_A;  /* = Not assignment */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0020;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short CTSQ:3;
            unsigned short VALID:1;
            unsigned short DVSQ:3;
            unsigned short VBSTS:1;
            unsigned short BRDY:1;
            unsigned short NRDY:1;
            unsigned short BEMP:1;
            unsigned short CTRT:1;
            unsigned short DVST:1;
            unsigned short SOFR:1;
            unsigned short RESM:1;
            unsigned short VBINT:1;
        } BIT;
    } INTSTS0;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0021;
#endif
    union{
        unsigned short WORD;
        struct{
            unsigned short PDDETINT:1;
            unsigned short :3;
            unsigned short SACK:1;
            unsigned short SIGN:1;
            unsigned short EOFERR:1;
            unsigned short :4;
            unsigned short ATTCH:1;
            unsigned short DTCH:1;
            unsigned short :1;
            unsigned short BCHG:1;
            unsigned short OVRCR:1;
        }BIT;
    }INTSTS1;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0022;
#endif
    union {
        unsigned short WORD;
        struct{
            unsigned short PDDETINT:1;
            unsigned short :5;
            unsigned short EOFERRE:1;
            unsigned short :4;
            unsigned short ATTCHE:1;
            unsigned short DTCHE:1;
            unsigned short :1;
            unsigned short BCHGE:1;
            unsigned short OVRCRE:1;
        }BIT;
    } INTSTS2;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0023;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short PIPE0BRDY:1;
            unsigned short :3;
            unsigned short PIPE4BRDY:1;
            unsigned short PIPE5BRDY:1;
            unsigned short PIPE6BRDY:1;
            unsigned short PIPE7BRDY:1;
            unsigned short :8;
        } BIT;
    } BRDYSTS;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0024;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short PIPE0NRDY:1;
            unsigned short :3;
            unsigned short PIPE4NRDY:1;
            unsigned short PIPE5NRDY:1;
            unsigned short PIPE6NRDY:1;
            unsigned short PIPE7NRDY:1;
            unsigned short :8;
        } BIT;
    } NRDYSTS;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0025;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short PIPE0BENP:1;
            unsigned short :3;
            unsigned short PIPE4BENP:1;
            unsigned short PIPE5BENP:1;
            unsigned short PIPE6BENP:1;
            unsigned short PIPE7BENP:1;
            unsigned short :8;
        } BIT;
    } BEMPSTS;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0026;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short FRNM:11;
            unsigned short :5;
        } BIT;
    } FRMNUM;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0027;
#endif
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_B;  /* = Not assignment */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0028;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short USBADDR:7;
            unsigned short :9;
        } BIT;
    } USBADDR;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0029;
#endif
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_C;  /* = Not assignment */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No002A;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short BMREQUESTTYPE:8;
            unsigned short BREQUEST:8;
        } BIT;
    } USBREQ;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No002B;
#endif
    union {
        unsigned short WORD;
    } USBVAL;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No002C;
#endif
    union {
        unsigned short WORD;
    } USBINDX;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No002D;
#endif
    union {
        unsigned short WORD;
    } USBLENG;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No002E;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short :4;
            unsigned short DIR:1;
            unsigned short :2;
            unsigned short SHTNAK:1;
            unsigned short :8;
        } BIT;
    } DCPCFG;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No002F;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short MXPS:7;
            unsigned short :5;
            unsigned short DEVSEL:3;
            unsigned short :1;
        } BIT;
    } DCPMAXP;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0030;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short PID:2;
            unsigned short CCPL:1;
            unsigned short :2;
            unsigned short PBUSY:1;
            unsigned short SQMON:1;
            unsigned short SQSET:1;
            unsigned short SQCLR:1;
            unsigned short :2;
            unsigned short SUREQCLR:1;
            unsigned short :2;
            unsigned short SUREQ:1;
            unsigned short BSTS:1;
        } BIT;
    } DCPCTR;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0031;
#endif
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_D;  /* = Not assignment */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0032;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short PIPESEL:4;
            unsigned short :12;
        } BIT;
    } PIPESEL;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0033;
#endif
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_E;  /* = Not assignment */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0034;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short EPNUM:4;
            unsigned short DIR:1;
            unsigned short :2;
            unsigned short SHTNAK:1;
            unsigned short :1;
            unsigned short DBLB:1;
            unsigned short BFRE:1;
            unsigned short :3;
            unsigned short TYPE:2;
        } BIT;
    } PIPECFG;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0035;
#endif
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_F;  /* = Not assignment */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0036;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short MXPS:9;
            unsigned short :3;
            unsigned short DEVSEL:3;
            unsigned short :1;
        } BIT;
    } PIPEMAXP;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0037;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short IITV:3;
            unsigned short :13;
        } BIT;
    } PIPEPERI;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0038;
#endif

#ifdef  __RX
    union {
        unsigned short WORD;
        struct {
            unsigned short PID:2;
            unsigned short :3;
            unsigned short PBUSY:1;
            unsigned short SQMON:1;
            unsigned short SQSET:1;
            unsigned short SQCLR:1;
            unsigned short ACLRM:1;
            unsigned short ATREPM:1;
            unsigned short :3;
            unsigned short INBUFM:1;
            unsigned short BSTS:1;
        } BIT;
    } PIPE1CTR;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0039;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short PID:2;
            unsigned short :3;
            unsigned short PBUSY:1;
            unsigned short SQMON:1;
            unsigned short SQSET:1;
            unsigned short SQCLR:1;
            unsigned short ACLRM:1;
            unsigned short ATREPM:1;
            unsigned short :3;
            unsigned short INBUFM:1;
            unsigned short BSTS:1;
        } BIT;
    } PIPE2CTR;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No003A;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short PID:2;
            unsigned short :3;
            unsigned short PBUSY:1;
            unsigned short SQMON:1;
            unsigned short SQSET:1;
            unsigned short SQCLR:1;
            unsigned short ACLRM:1;
            unsigned short ATREPM:1;
            unsigned short :3;
            unsigned short INBUFM:1;
            unsigned short BSTS:1;
        } BIT;
    } PIPE3CTR;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No003B;
#endif

#else

    union {
        unsigned short WORD;
    } USB_REG_DUMMY_10; /* = Not assignment */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0039;
#endif
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_11; /* = Not assignment */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No003A;
#endif
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_12; /* = Not assignment */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No003B;
#endif

#endif  /* __RX */

    union {
        unsigned short WORD;
        struct {
            unsigned short PID:2;
            unsigned short :3;
            unsigned short PBUSY:1;
            unsigned short SQMON:1;
            unsigned short SQSET:1;
            unsigned short SQCLR:1;
            unsigned short ACLRM:1;
            unsigned short ATREPM:1;
            unsigned short :3;
            unsigned short INBUFM:1;
            unsigned short BSTS:1;
        } BIT;
    } PIPE4CTR;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No003C;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short PID:2;
            unsigned short :3;
            unsigned short PBUSY:1;
            unsigned short SQMON:1;
            unsigned short SQSET:1;
            unsigned short SQCLR:1;
            unsigned short ACLRM:1;
            unsigned short ATREPM:1;
            unsigned short :3;
            unsigned short INBUFM:1;
            unsigned short BSTS:1;
        } BIT;
    } PIPE5CTR;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No003D;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short PID:2;
            unsigned short :3;
            unsigned short PBUSY:1;
            unsigned short SQMON:1;
            unsigned short SQSET:1;
            unsigned short SQCLR:1;
            unsigned short ACLRM:1;
            unsigned short :5;
            unsigned short BSTS:1;
        } BIT;
    } PIPE6CTR;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No003E;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short PID:2;
            unsigned short :3;
            unsigned short PBUSY:1;
            unsigned short SQMON:1;
            unsigned short SQSET:1;
            unsigned short SQCLR:1;
            unsigned short ACLRM:1;
            unsigned short :5;
            unsigned short BSTS:1;
        } BIT;
    } PIPE7CTR;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No003F;
#endif

#ifdef  __RX
    union {
        unsigned short WORD;
        struct {
            unsigned short PID:2;
            unsigned short :3;
            unsigned short PBUSY:1;
            unsigned short SQMON:1;
            unsigned short SQSET:1;
            unsigned short SQCLR:1;
            unsigned short ACLRM:1;
            unsigned short :5;
            unsigned short BSTS:1;
        } BIT;
    } PIPE8CTR;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0040;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short PID:2;
            unsigned short :3;
            unsigned short PBUSY:1;
            unsigned short SQMON:1;
            unsigned short SQSET:1;
            unsigned short SQCLR:1;
            unsigned short ACLRM:1;
            unsigned short :5;
            unsigned short BSTS:1;
        } BIT;
    } PIPE9CTR;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0041;
#endif

#else

    union {
        unsigned short WORD;
    } USB_REG_DUMMY_13; /* = Not assignment */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0040;
#endif
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_14; /* = Not assignment */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0041;
#endif

#endif  /* __RX */

    union {
        unsigned short WORD;
    } USB_REG_DUMMY_15; /* = Not assignment */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0042;
#endif
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_16; /* = Not assignment */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0043;
#endif
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_17; /* = Not assignment */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0044;
#endif
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_18; /* = Not assignment */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0045;
#endif
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_19; /* = Not assignment */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0046;
#endif
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_1A; /* = Not assignment */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0047;
#endif
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_1B; /* = Not assignment */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0048;
#endif

#ifdef  __RX
    union {
        unsigned short WORD;
        struct {
            unsigned short :8;
            unsigned short TRCLR:1;
            unsigned short TRENB:1;
            unsigned short :6;
        } BIT;
    } PIPE1TRE;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0049;
#endif
    union {
        unsigned short WORD;
    } PIPE1TRN;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No004A;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short :8;
            unsigned short TRCLR:1;
            unsigned short TRENB:1;
            unsigned short :6;
        } BIT;
    } PIPE2TRE;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No004B;
#endif
    union {
        unsigned short WORD;
    } PIPE2TRN;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No004C;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short :8;
            unsigned short TRCLR:1;
            unsigned short TRENB:1;
            unsigned short :6;
        } BIT;
    } PIPE3TRE;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No004D;
#endif
    union {
        unsigned short WORD;
    } PIPE3TRN;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No004E;
#endif
    
#else

    union {
        unsigned short WORD;
    } USB_REG_DUMMY_1C; /* = Not assignment */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0049;
#endif
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_1D; /* = Not assignment */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No004A;
#endif
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_1E; /* = Not assignment */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No004B;
#endif
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_1F; /* = Not assignment */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No004C;
#endif
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_20; /* = Not assignment */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No004D;
#endif
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_21; /* = Not assignment */
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No004E;
#endif
#endif  /* __RX */

    union {
        unsigned short WORD;
        struct {
            unsigned short :8;
            unsigned short TRCLR:1;
            unsigned short TRENB:1;
            unsigned short :6;
        } BIT;
    } PIPE4TRE;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No004F;
#endif
    union {
        unsigned short WORD;
    } PIPE4TRN;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0050;
#endif
    union {
        unsigned short WORD;
        struct {
            unsigned short :8;
            unsigned short TRCLR:1;
            unsigned short TRENB:1;
            unsigned short :6;
        } BIT;
    } PIPE5TRE;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0051;
#endif
    union {
        unsigned short WORD;
    } PIPE5TRN;
    unsigned short  usb_hole_No0052;
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_22; /* = Not assignment */
    unsigned short  usb_hole_No0053;
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_23; /* = Not assignment */
    unsigned short  usb_hole_No0054;
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_24; /* = Not assignment */
    union {
        unsigned short WORD;
        struct {
            unsigned short RPDME0:1;
            unsigned short IDPSRCE0:1;
            unsigned short IDMSINKE0:1;
            unsigned short VDPSRCE0:1;
            unsigned short IDPSINKE0:1;
            unsigned short VDMSRCE0:1;
            unsigned short DPMODE0:1;
            unsigned short BATCHGE0:1;
            unsigned short CHGDETSTS0:1;
            unsigned short PDDETSTS0:1;
            unsigned short :6;
        } BIT;
    } USBBCCTRL0;
    unsigned short  usb_hole_No0055;
    union {
        unsigned short WORD;
        struct {
            unsigned short RPDME1:1;
            unsigned short IDPSRCE1:1;
            unsigned short IDMSINKE1:1;
            unsigned short VDPSRCE1:1;
            unsigned short IDPSINKE1:1;
            unsigned short VDMSRCE1:1;
            unsigned short DPMODE1:1;
            unsigned short BATCHGE1:1;
            unsigned short CHGDETSTS1:1;
            unsigned short PDDETSTS1:1;
            unsigned short :6;
        } BIT;
    } USBBCCTRL1;
    unsigned short  usb_hole_No0056;
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_XXX;    /* = Not assignment */
    unsigned short  usb_hole_No0057;
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_XX; /* = Not assignment */
    unsigned short  usb_hole_No0058;
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_25; /* = Not assignment */
    unsigned short  usb_hole_No0059;
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_26; /* = Not assignment */
    unsigned short  usb_hole_No005A;
    union {
        unsigned short WORD;
    } USB_REG_DUMMY_27; /* = Not assignment */
    unsigned short  usb_hole_No005B;
    union {
        unsigned short WORD;
        struct {
            unsigned short VDDUSBE:1;
            unsigned short PXXCON:1;
            unsigned short :5;
            unsigned short VBRPDCUT:1;
            unsigned short :8;
        } BIT;
    } USBMC;
    unsigned short  usb_hole_No005C;

    union{
        unsigned short WORD;
        struct{
            unsigned short :6;
            unsigned short USBSPD:2;
            unsigned short :8;
        }BIT;
    }DEVADD0;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No005D;
#endif
    union{
        unsigned short WORD;
        struct{
            unsigned short :6;
            unsigned short USBSPD:2;
            unsigned short :8;
        }BIT;
    }DEVADD1;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No005E;
#endif
    union{
        unsigned short WORD;
        struct{
            unsigned short :6;
            unsigned short USBSPD:2;
            unsigned short :8;
        }BIT;
    }DEVADD2;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No005F;
#endif
    union{
        unsigned short WORD;
        struct{
            unsigned short :6;
            unsigned short USBSPD:2;
            unsigned short :8;
        }BIT;
    }DEVADD3;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0060;
#endif
    union{
        unsigned short WORD;
        struct{
            unsigned short :6;
            unsigned short USBSPD:2;
            unsigned short :8;
        }BIT;
    }DEVADD4;
#if defined(USB_REG_ALIGN_4)
    unsigned short  usb_hole_No0061;
#endif
    union{
        unsigned short WORD;
        struct{
            unsigned short :6;
            unsigned short USBSPD:2;
            unsigned short :8;
        }BIT;
    }DEVADD5;
};

#endif  /* __R_USB_IODEFINE_H__ */
/******************************************************************************
End Of File
******************************************************************************/
