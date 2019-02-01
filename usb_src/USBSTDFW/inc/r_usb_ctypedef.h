/*******************************************************************************
* File Name    : r_usb_ctypedef.h
* Version      : 2.10
* Description  : Type definition
*******************************************************************************/
/*******************************************************************************
* History   : DD.MM.YYYY Version Description
*           : 31.03.2012 1.80    Basic FW mini Ver2.00(ES Version)
*           : 30.11.2012 2.00    Basic FW mini Ver2.00 Release
*           : 31.01.2013 2.01    Corresponded to RL78/L1C
*           : 01.08.2013 2.10    Basic Mini FW Ver2.10 Release
*******************************************************************************/
#ifndef __R_USB_CTYPEDEF_H__
#define __R_USB_CTYPEDEF_H__


/******************************************************************************
Macro definitions
******************************************************************************/


/******************************************************************************
Typedef definitions
******************************************************************************/
//typedef unsigned char       uint8_t;    /*  8bit */
//typedef unsigned short      uint16_t;   /* 16bit */
//typedef unsigned long       uint32_t;   /* 32bit */
//typedef signed char         int8_t;
//typedef signed short        int16_t;
//typedef signed long         int32_t;
#include        <r_cg_macrodriver.h>
typedef uint8_t             usb_pipe_t;
typedef uint8_t             usb_port_t;
typedef uint8_t             usb_addr_t;
typedef uint16_t            usb_leng_t;
typedef uint8_t             usb_strct_t;

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef struct  usb_sutr_t  usb_utr_t;
typedef void    (*usb_cbinfo_t)(uint16_t, uint16_t);
typedef void    (*usb_cbcheck_t)(uint8_t**);
typedef void    (*usb_cb_t)(usb_utr_t*);
typedef void    (*mgrfunc_t)(void);


/******************************************************************************
Struct definitions
******************************************************************************/
typedef struct
{
    union {
        struct {                    /* Characteristics of request */
#ifdef __BIG
            /* Big-endian */    
            uint8_t bRequest:8;     /* Specific request */
            uint8_t bRecipient:5;       /* Recipient */
            uint8_t bType:2;        /* Type */
            uint8_t bDirection:1;       /* Data transfer direction */
#else
            /* Little-endian */
            uint8_t bRecipient:5;       /* Recipient */
            uint8_t bType:2;        /* Type */
            uint8_t bDirection:1;       /* Data transfer direction */
            uint8_t bRequest:8;     /* Specific request */
#endif
        } BIT;
        uint16_t    wRequest;       /* Control transfer request */
    } WORD;
    uint16_t        wValue;         /* Control transfer value */
    uint16_t        wIndex;         /* Control transfer index */
    uint16_t        wLength;        /* Control transfer length */
} usb_request_t;
typedef void    (*usb_cb_trn_t)(usb_request_t*, uint16_t);

typedef struct
{
    union {
        struct {                    /* Characteristics of request */
#ifdef __BIG
            /* Big-endian */
            uint8_t bRequest;       /* Characteristics of request */
            uint8_t bmRequestType;      /* Specific request */
#else
            /* Little-endian */
            uint8_t bmRequestType;      /* Characteristics of request */
            uint8_t bRequest;           /* Specific request */
#endif
        } BYTE;
        uint16_t    wRequest;       /* Control transfer request */
    } WORD;
    uint16_t        wValue;         /* Control transfer value */
    uint16_t        wIndex;         /* Control transfer index */
    uint16_t        wLength;        /* Control transfer length */
    uint16_t        Address;
} usb_hcdrequest_t;

typedef struct
{
    uint16_t        *pipetbl;       /* Pipe Define Table address */
    uint8_t         *devicetbl;     /* Device descriptor Table address */
    uint8_t         *configtbl;     /* Configuration descriptor Table address */
    uint8_t         **stringtbl;    /* String descriptor Table address */
    usb_cbinfo_t    statediagram;   /* Device status */
    usb_cb_trn_t    ctrltrans;      /* Control Transfer */
} usb_pcdreg_t;

typedef struct
{
    usb_port_t      rootport;       /* Root port */
    usb_addr_t      devaddr;        /* Device address */
    uint8_t         devstate;       /* Device state */
    uint8_t         ifclass;        /* Interface Class */
    usb_cbcheck_t   classcheck;     /* Driver check */
    usb_cbinfo_t    statediagram;   /* Device status */
} usb_hcdreg_t;

typedef struct
{
    usb_strct_t     msginfo;        /* Message Info for F/W */
    usb_strct_t     inttype;        /* Interrupt type */
#ifdef  __RX
    uint16_t        bitsts;         /* Fifo buffer pipe status */
#else   /* __RX */
    usb_strct_t     bitsts;         /* Fifo buffer pipe status */
#endif  /* __RX */
    usb_strct_t     dummy;          /* not used */
} usb_intinfo_t;

typedef struct
{
    usb_strct_t     msginfo;        /* Message Info for F/W */
    usb_strct_t     keyword;        /* Rootport / Device address / Pipe number */
#ifdef  __RX
    uint16_t        status;         /* status */
#else   /* __RX */
    usb_strct_t     status;         /* status */
#endif  /* __RX */
    usb_strct_t     flag;           /* Message enable/disable */
    usb_cbinfo_t    complete;       /* Device status */
} usb_tskinfo_t;

struct usb_sutr_t
{
    usb_strct_t     msginfo;        /* Message Info for F/W */
    usb_strct_t     pipenum;        /* Pipe number */
#ifdef  __RX
    uint16_t        status;         /* status */
#else   /* __RX */
    usb_strct_t     status;         /* Transfer status */
#endif  /* __RX */
    usb_strct_t     flag;           /* not used */
    usb_cb_t        complete;       /* Callback function address */
    uint8_t         *tranadr;       /* Transfer data start address */
    uint16_t        *setup;         /* Setup packet(for control only) */
    uint16_t        pipectr;        /* Pipe control register */
    usb_leng_t      tranlen;        /* Transfer data length */
    uint16_t        dummy;          /* not used */
};

typedef struct
{
    usb_addr_t      port;           /* root port */
    uint8_t         state;          /* Device state */
    uint8_t         config;         /* Configuration number */
    uint8_t         speed;          /* Device speed */
} usb_hcddevinf_t;

typedef struct
{
    uint8_t         dcpmode;        /* DCP Mode Flag */
    uint8_t         state;          /* BC State */
    uint8_t         pd_detect;      /* PD Detect Flag */
} usb_bc_status_t;

typedef struct{
    union {
        uint16_t    w;
        struct {
#ifdef __BIG
            /* Big-endian */
            uint8_t up;
            uint8_t dn;
#else
            /* Little-endian */
            uint8_t dn;
            uint8_t up;
#endif
        } BYTE;
    } WORD;
} status_t;

typedef struct{
    union{
        uint8_t b;
        struct {
            uint8_t PIPE0:1;
            uint8_t :7;
        } BIT;
    } BYTE;
} pipests_t;
typedef struct{
    union{
        uint8_t b;
        struct {
            uint8_t BRDY:1;
            uint8_t NRDY:1;
            uint8_t BEMP:1;
            uint8_t CTRT:1;
            uint8_t DVST:1;
            uint8_t SOFR:1;
            uint8_t RESM:1;
            uint8_t VBINT:1;
        } BIT;
    } BYTE;
} intbit_t;
typedef struct{
    union{
        uint16_t    w;
        struct {
#ifdef __BIG
            /* Big-endian */
            uint8_t :3;
            uint8_t ATTCH:1;
            uint8_t DTCH:1;
            uint8_t :1;
            uint8_t BCHG:1;
            uint8_t OVRCR:1;
            uint8_t PDDETINT:1;
            uint8_t :3;
            uint8_t SACK:1;
            uint8_t SIGN:1;
            uint8_t EOFERR:1;
            uint8_t :1;
#else
            /* Little-endian */
            uint8_t PDDETINT:1;
            uint8_t :3;
            uint8_t SACK:1;
            uint8_t SIGN:1;
            uint8_t EOFERR:1;
            uint8_t :1;
            uint8_t :3;
            uint8_t ATTCH:1;
            uint8_t DTCH:1;
            uint8_t :1;
            uint8_t BCHG:1;
            uint8_t OVRCR:1;
#endif
        } BIT;
    } WORD;
} intbit2_t;

typedef struct{
    union {
        uint16_t    w;
        struct {
#ifdef __BIG
            /* Big-endian */
            uint8_t up;
            uint8_t dn;
#else
            /* Little-endian */
            uint8_t dn;
            uint8_t up;
#endif
        } BYTE;
    } WORD;
} totallen_t;



#endif  /* __R_USB_CTYPEDEF_H__ */
/******************************************************************************
End of file
******************************************************************************/
