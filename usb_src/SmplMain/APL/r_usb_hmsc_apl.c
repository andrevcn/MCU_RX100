/******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized.
* This software is owned by Renesas Electronics Corporation and is  protected
* under all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING
* BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE  LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this
* software and to discontinue the availability of this software.
*    By using this software, you agree to the additional terms and conditions
* found by accessing the following link:
* http://www.renesas.com/disclaimer
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************
* File Name    : r_usb_hmsc_apl.c
* Version      : 2.10
* Description  : USB Host Sample Code
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 01.08.2013 2.10    Basic Mini FW Ver2.10 Release
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_cTypedef.h"         /* Type define */
#include "r_usb_usrconfig.h"        /* USB-H/W register set (user define) */
#include "r_usb_cDefUSBIP.h"        /* USB-FW Library Header */
#include "r_usb_cKernelId.h"        /* Kernel ID definition */
#include <stdlib.h>                 /* HMSC Sample use */
#include <stdio.h>                  /* HMSC Sample use */
#include "r_usb_hmsc_define.h"      /* HMSC define */
#include "r_TinyFAT.h"              /* TFAT header */

#include "r_usb_hmsc_extern.h"
#include "hw_resource.h"
#include "r_usb_api.h"
#include "r_usb_hmsc_api.h"
#include "r_user_hwrctrl.h"

/*****************************************************************************
Macro definitions
******************************************************************************/
#define USB_HMSC_DRIVEMOUNT         (usb_strct_t)0x10
#define USB_HMSC_FILEREAD           (usb_strct_t)0x11
#define USB_HMSC_FILEWRITE          (usb_strct_t)0x12
#define USB_HMSC_DRIVE_OPEN         (usb_strct_t)0x13
#define USER_SW_PRESS1               (usb_strct_t)0x16
#define USB_DEMO_START              (usb_strct_t)0x17
#define USER_SW_PRESS2               (usb_strct_t)0x18
#define USB_APL_CLR                 (usb_strct_t)0x19

/* SW */
#define USB_NO_PUSH                 0u
#define USB_SW1_PUSH                1u
#define USB_SW2_PUSH                2u
#define USB_SW3_PUSH                4u
/* Demonstration */
#define USB_TRANSFER_START          0u
#define USB_TRANSFER_STOP           1u

/******************************************************************************
Section    <Section Definition> , "Data Sections"
******************************************************************************/
/* Condition compilation by the difference of hardware reference */
#ifdef  R8CUSB
#pragma section program P_apl
#pragma section bss B_hmsc
#endif
#ifdef  RL78USB
#endif
#ifdef  RL78USB_SECTION
#pragma section @@CODE   P_APL
#pragma section @@BASE   T_APL
#pragma section @@DATA   B_APL
#endif

/******************************************************************************
Private global variables and functions
******************************************************************************/
uint16_t    usb_ghmsc_SmpAplProcess;
uint16_t    usb_shmsc_DriveOpenSeq = USB_SEQ_0;
uint16_t    usb_shmsc_demo_state = USB_TRANSFER_STOP;
//uint8_t     usb_gBuf[512]; /* ALISON - TEMP */
uint8_t     usb_gBuf;
uint16_t POS_DA_MENSAGEM; //SERÁ LOCAL
/* ALISON - TEMP */
//extern __root uint8_t RAW_MSG[MSG_MAX_NUMBER][MENSAGEM_MAX_TAMANHO];

/* Variáveis externas */
extern void Sys_ConvertASCIItoMatriz(uint32_t MsgNumber);
extern __root  uint16_t                 MAT_MSG_INFO[MSG_MAX_NUMBER][10];
extern          uint16_t        Sys_TypeOfMsgOnDisplay;
extern          uint32_t        Sys_DisplayFlags; /* Flags de controle do display */
extern          uint32_t        Sys_MessagePointer;
extern          uint16_t        Sys_InfoDelay;
extern          uint16_t        Sys_InfoDelayTime;
extern void FDL_WriteControl (void);
/* ALISON - TEMP */

FATFS       usb_gFatfs;
uint16_t    usb_sLedwork1 = 0u;
uint16_t    usb_sLedwork2 = 1u;

void        usb_hmsc_apl_init();
void        usb_hmsc_AplClear();
void        usb_shmsc_DemoStateChange(void);
static void usb_hapl_registration();
void        usb_hmsc_DriveClose(uint16_t addr, uint16_t data2);
void        usb_hmsc_DriveOpen(uint16_t addr, uint16_t data2);
void        usb_hmsc_task_start( void );
void        usb_hmsc_SampleAplTask(void);
void        usb_hmsc_SampleAplSpecifiedPath(usb_tskinfo_t *mess);
USB_STATIC void usb_hmsc_device_state(uint16_t data, uint16_t state);

int16_t     usb_ghmsc_open = USB_E_ERROR;

const uint16_t usb_gapl_devicetpl[] =
{
  /* Number of tpl table */
  4,
  /* Reserved */
  0,
  /* Vendor ID  : no-check */
  USB_NOVENDOR,
  /* Product ID : no-check */
  USB_NOPRODUCT,
};

/******************************************************************************
External variables and functions
******************************************************************************/
//extern void         usb_cpu_LcdDisp(uint8_t position, uint8_t *string);
//extern void         usb_cpu_led_set_data(uint8_t data);
//extern void         usb_cpu_led_set_bit(uint8_t bit, uint8_t data);
//extern uint8_t      usb_smpl_key_input(void);

extern void         usb_cstd_dummy_function(uint16_t data1, uint16_t data2);
extern void LimpaTela(void);

/******************************************************************************
Renesas Host MSC Sample Code functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_task_start
Description     : Start task processing.
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hmsc_task_start( void )
{
  /* USB-IP initialized */

  /* HCD driver open & registratuion */
  usb_hapl_registration();        /* Host application registration. */
  R_usb_hmsc_driver_start();      /* Host class driver. */
  usb_hmsc_apl_init();            /* Clear application. */

  /* Scheduler initialized */
  R_usb_hstd_ChangeDeviceState(USB_DO_SETHWFUNCTION);
}   /* eof usb_hmsc_task_start() */

/******************************************************************************
Function Name   : usb_hapl_registration
Description     : Callback registration.
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hapl_registration()
{
  usb_hcdreg_t    driver;

  /* Driver registration */
  driver.ifclass      = (uint16_t)USB_IFCLS_MAS;      /* Use Interface class for MSC. */
  driver.classcheck   = &R_usb_hmsc_ClassCheck;       /* Driver check. */
  driver.statediagram = &usb_hmsc_device_state;

  /* Host MSC class driver registration. */
  R_usb_hstd_DriverRegistration(&driver);
}   /* eof usb_hapl_registration() */

/******************************************************************************
Function Name   : usb_hmsc_DriveOpen
Description     : drive open
Arguments       : uint16_t addr :
: uint16_t data2 :
Return value    : none
******************************************************************************/
void usb_hmsc_DriveOpen( uint16_t addr, uint16_t data2)
{
  /* Condition compilation by the difference of the File system */
#ifdef USB_TFAT_USE_PP
  usb_ghmsc_open = USB_E_OK;

  R_USB_GET_SND(USB_HMSCSMP_MBX, USB_HMSC_DRIVE_OPEN, &usb_cstd_dummy_function, (usb_strct_t)addr);
#else   /* !USB_TFAT_USE_PP */
  uint16_t    i, max_drv, result;
  int16_t     err;

  R_usb_hmsc_StrgDriveSearch( addr);

  if( result == USB_E_OK )
  {
    R_usb_hmsc_SetDevSts((uint16_t)USB_HMSC_DEV_ATT);
    for( i = USB_DRIVE; i < USB_MAXDRIVE; i++ )
    {
      if( usb_hmsc_SmpCheckAddr(i) == addr )
      {
        if( (err = dev_att((int16_t)i)) != 0 )
        {
        }
      }
    }
  }
  usb_hmsc_DrivePrint();
#endif  /* USB_TFAT_USE_PP */
}   /* eof usb_hmsc_DriveOpen() */

/******************************************************************************
Function Name   : usb_hmsc_DriveClose
Description     : drive close
Arguments       : uint16_t addr :
: uint16_t data2 :
Return value    : none
******************************************************************************/
void usb_hmsc_DriveClose( uint16_t addr, uint16_t data2)
{
  /* Condition compilation by the difference of the File system */
#ifdef USB_TFAT_USE_PP
  usb_hmsc_AplClear();
  R_usb_hmsc_SetDevSts((uint16_t)USB_HMSC_DEV_DET);
  usb_hmsc_SmpFsiDriveClear( addr);

#else   /* !USB_TFAT_USE_PP */
  uint16_t    i, min_drv, max_drv;

  for( i = USB_DRIVE; i < USB_MAXDRIVE; i++ )
  {
    if( usb_hmsc_SmpCheckAddr(i) == addr )
    {
      usb_hmsc_Det((int16_t)i);
    }
  }

  R_usb_hmsc_SetDevSts( (uint16_t)USB_HMSC_DEV_DET);
  usb_hmsc_SmpFsiDriveClear( addr);
  usb_hmsc_DrivePrint();
#endif  /* USB_TFAT_USE_PP */
}   /* eof usb_hmsc_DriveClose() */

/******************************************************************************
Function Name   : usb_hmsc_apl_init
Description     : Clear application.
Arguments       : none
Return          : none
******************************************************************************/
void usb_hmsc_apl_init()
{
  /* Demo stop */
  usb_shmsc_demo_state = USB_TRANSFER_STOP;

  /* Init this application's drive open sequence state. */
  usb_shmsc_DriveOpenSeq = USB_SEQ_0;
  usb_ghmsc_SmpAplProcess = USB_APL_CLR;
}   /* eof usb_hmsc_apl_init() */

/******************************************************************************
Function Name   : usb_hmsc_AplClear
Description     : Application clear
Arguments       : none
Return          : none
******************************************************************************/
void usb_hmsc_AplClear()
{
  usb_tskinfo_t dummy_mess;

  usb_hmsc_apl_init();            /* Clear application */
  usb_ghmsc_open = USB_E_ERROR;
  usb_hmsc_SampleAplSpecifiedPath( (usb_tskinfo_t*)&dummy_mess );
}   /* eof usb_hmsc_AplClear() */

/******************************************************************************
Function Name   : usb_shmsc_DemoStateChange
Description     : Change demo state.
Arguments       : none
Return value    : none
******************************************************************************/
void usb_shmsc_DemoStateChange(void)
{
  /* Application demo state. */
  if(usb_shmsc_demo_state == USB_TRANSFER_START)
  {
    /* Application demo state: Start->Stop. */
    usb_shmsc_demo_state = USB_TRANSFER_STOP;
  }
  else if(usb_shmsc_demo_state == USB_TRANSFER_STOP)
  {
    /* Application demo state: Stop->Start. */
    usb_shmsc_demo_state = USB_TRANSFER_START;
  }
  else
  {
  }
}   /* eof usb_shmsc_DemoStateChange() */

/******************************************************************************
Function Name   : usb_hmsc_SampleAplTask
Description     : Sample application; "hmscdemo.txt" read-write loop.
: Write once the file "hmscdemo.txt" to the USB memory media.
: Demo application reads data repeatedly. Demo start/stop switch
: is SW2 for RSK.
Argument        : none
Return          : none
******************************************************************************/
extern void Sys_ConvertASCIItoMatriz(uint32_t MsgNumber);
void usb_hmsc_SampleAplTask(void)
{
  usb_tskinfo_t   *mess;
  usb_er_t        err;
  FRESULT         res;
  usb_tskinfo_t   *mes;
  uint16_t        addr;
  FIL             file;
  uint16_t        file_rw_cnt;
  int16_t         file_err = USB_E_OK;
  extern unsigned char ERRO_USB;
  if(Sys_TypeOfMsgOnDisplay != NormalMessage)
  {
    return;
  }
  /* Check for incoming application messages. */
  err = R_USB_RCV_MSG(USB_HMSCSMP_MBX, (usb_msg_t**)&mess);
  if( err != USB_E_OK )
  {
    return;
  }
  /* Switch action depending on message. */
  mes = (usb_tskinfo_t *)mess;
  switch( mes->msginfo )
  {
  case USB_HMSC_DRIVE_OPEN:
    /* Message next time we come back here is.. */
    /* == 0 -> demo start */
    usb_shmsc_demo_state = USB_TRANSFER_START;
    usb_ghmsc_SmpAplProcess = USB_DEMO_START;
    usb_hmsc_SampleAplSpecifiedPath(mes);
    break;

  case USER_SW_PRESS1:
    /* Check demo status. */
    if( usb_shmsc_demo_state != 0 )
    {
      /* Message next time we come back here is.. */
      /* If is 1 -> wait. */
      usb_hmsc_SampleAplSpecifiedPath(mes);
    }
    else
    {
      /* Message next time we come back here is Mount device. */
      usb_ghmsc_SmpAplProcess = USB_HMSC_DRIVEMOUNT;
      usb_hmsc_SampleAplSpecifiedPath(mes);
    }
    break;

  case USB_DEMO_START:
    /* Switch on application drive open sequence state. */
    switch( usb_shmsc_DriveOpenSeq )
    {
    case USB_SEQ_0:
      /* Increment application drive open sequence. */
      usb_shmsc_DriveOpenSeq++;

      /* Set device address. */
      addr = mes->keyword;

      /* Storage drive search. */
      R_usb_hmsc_StrgDriveSearch(addr);
      break;
    case USB_SEQ_1:
      /* Check storage drive search result. */
      if( mes->keyword == USB_E_OK )
      {
        /* Device status setting = attached. */
        R_usb_hmsc_SetDevSts((uint16_t)USB_HMSC_DEV_ATT);

        /* Application initialization sequence start. */

        /* Increment application drive open sequence. */
        usb_shmsc_DriveOpenSeq = USB_SEQ_0;

        /* Send message to myself: Start demo key input wait. */
        usb_ghmsc_SmpAplProcess = USER_SW_PRESS1;
        usb_hmsc_SampleAplSpecifiedPath(mes);
      }
      break;
    default:
      /* Reset application drive open sequence. */
      usb_shmsc_DriveOpenSeq = USB_SEQ_0;
      break;
    }
    break;

  case USB_HMSC_DRIVEMOUNT:
    /* File system media work area memory mount. */
    res = R_tfat_f_mount(0, &usb_gFatfs);
    if( res != TFAT_FR_OK )
    {
      while(1);
    }
    /* Message next time we come back here is.. */
    //    usb_ghmsc_SmpAplProcess = USB_HMSC_FILEWRITE;
    usb_ghmsc_SmpAplProcess = USB_HMSC_FILEREAD;
    usb_hmsc_SampleAplSpecifiedPath(mes);
    break;

  case USER_SW_PRESS2:
    /* Check demo status. */
    if( usb_shmsc_demo_state != 0 )
    {
      /* Message next time we come back here is.. */
      /* == 1 -> wait */
      usb_hmsc_SampleAplSpecifiedPath(mes);
    }
    else
    {
      /* Message next time we come back here is.. */
      /* == 0 -> demo start */
      usb_ghmsc_SmpAplProcess = USB_HMSC_FILEREAD;
      usb_hmsc_SampleAplSpecifiedPath(mes);

    }
    break;

  case USB_HMSC_FILEREAD:
    if( usb_ghmsc_open == USB_E_OK )
    {
      usb_ghmsc_SmpAplProcess = USB_APL_CLR;
      /* File open in read mode */
      res = R_tfat_f_open(&file, "pev.veh", (TFAT_FA_OPEN_ALWAYS | TFAT_FA_READ));
      /* Move pointer in file object. */
      res = R_tfat_f_lseek(&file, 0);
      //======================================================================//
      //uint32_t POS_DA_MENSAGEM;
      uint32_t MSG_A_SER_LIDA;
      uint8_t CALCULO;
      uint8_t ARQUIVO_VALIDO = 0;
      uint8_t tempTacL;
      uint8_t tempTacH;
      file_rw_cnt = 1; /* File Read Counter */
      //for (uint16_t i=0;i<3072;i++)
      //  flash.FLASH_VECTOR[i]=0;
      for(MSG_A_SER_LIDA = 0;(MSG_A_SER_LIDA < 13);MSG_A_SER_LIDA++)//MARCOS && (file_rw_cnt != 0);MSG_A_SER_LIDA++)
      {
        usb_gBuf = 0; /* Armazenador de caractrer lido do arquivo */
        if (MSG_A_SER_LIDA<12)
        {
          for(POS_DA_MENSAGEM = 0; usb_gBuf != '\n';POS_DA_MENSAGEM++)
          {
            //dividir entre efeito e mensagem...
            /* Read data from file. */
            res = R_tfat_f_read(&file, &usb_gBuf, 1, &file_rw_cnt); /* ALISON - TEMP */
            if( res != TFAT_FR_OK )
            {
              usb_shmsc_demo_state = USB_TRANSFER_STOP;
              file_err = USB_E_ERROR;
            }
            if((POS_DA_MENSAGEM < 256) && file_rw_cnt)
            {//COLOCAR AQUI O LANCE DE APAGAR A MEMORIA
              if (!ARQUIVO_VALIDO)
              {
                tempTacL = flash.msg[0][252];
                tempTacH = flash.msg[0][251];
                for (uint16_t i=0;i<3072;i++)
                flash.FLASH_VECTOR[i]=0;
                ARQUIVO_VALIDO = 1;
              }
              if (POS_DA_MENSAGEM<1)
                  CALCULO = usb_gBuf-48;//PARA OS NUMEROS
              else if (POS_DA_MENSAGEM<2)
                flash.msg[MSG_A_SER_LIDA][240] = CALCULO*10+(usb_gBuf-48);//efeito
              else if (POS_DA_MENSAGEM<3)
                flash.msg[MSG_A_SER_LIDA][241] = usb_gBuf-48;//habilitada
              else if (POS_DA_MENSAGEM<4)
              {
                CALCULO = usb_gBuf-48;
                if (CALCULO==0)
                {
                  flash.msg[MSG_A_SER_LIDA][242] = 19;
                }
                else if (CALCULO==1)
                {
                  flash.msg[MSG_A_SER_LIDA][242] = 11;
                }
                else if (CALCULO==2)
                {
                  flash.msg[MSG_A_SER_LIDA][242] = 7;
                }
              }
              else if (POS_DA_MENSAGEM<5)
              {
                CALCULO = usb_gBuf-48;
                if (CALCULO==0)
                {
                  flash.msg[MSG_A_SER_LIDA][244] = 80;
                }
                else if (CALCULO==1)
                {
                  flash.msg[MSG_A_SER_LIDA][244] = 170;
                }
                else if (CALCULO==2)
                {
                  flash.msg[MSG_A_SER_LIDA][244] = 250;
                }
              }
              else if (POS_DA_MENSAGEM<6)
              {//orientação
                CALCULO = usb_gBuf-48;
                flash.msg[0][245] = CALCULO;
                flash.msg[1][245] = CALCULO;
                flash.msg[2][245] = CALCULO;
                flash.msg[3][245] = CALCULO;
                flash.msg[4][245] = CALCULO;
                flash.msg[5][245] = CALCULO;
                flash.msg[6][245] = CALCULO;
                flash.msg[7][245] = CALCULO;
                flash.msg[8][245] = CALCULO;
                flash.msg[9][245] = CALCULO;
                flash.msg[10][245] = CALCULO;
                flash.msg[11][245] = CALCULO;
              }
              else if (POS_DA_MENSAGEM<7)
              {
                flash.msg[MSG_A_SER_LIDA][246] = usb_gBuf-48;//ENTRADA QUE ACIONA A MENSAGEM
              }
              else if (POS_DA_MENSAGEM<8)
              {
                flash.msg[MSG_A_SER_LIDA][247] = usb_gBuf-48;//NUMERO DE REPETIÇÕES DA MENSAGEM
              }
              else if (POS_DA_MENSAGEM<9)
              {
                flash.msg[MSG_A_SER_LIDA][248] = usb_gBuf-48;//PRIORIDADE DA MENSAGEM
              }
              else if (POS_DA_MENSAGEM<10)
              {
                flash.msg[MSG_A_SER_LIDA][250] = usb_gBuf-48;//positivo ou negativo
              }
              else if (usb_gBuf != 10)
                flash.msg[MSG_A_SER_LIDA][POS_DA_MENSAGEM-10] = usb_gBuf;
            }
            if(!file_rw_cnt)
            {
              //ERRO_USB=1;
              //FDL_LoadControl();
              break;
            }
          }
        }
        else
        {
           for(POS_DA_MENSAGEM = 0; POS_DA_MENSAGEM < 2;POS_DA_MENSAGEM++)
           {
              res = R_tfat_f_read(&file, &usb_gBuf, 1, &file_rw_cnt); /* ALISON - TEMP */
              if( res != TFAT_FR_OK )
              {
                usb_shmsc_demo_state = USB_TRANSFER_STOP;
                file_err = USB_E_ERROR;
              }
              if (POS_DA_MENSAGEM<1)
                  CALCULO = usb_gBuf-48;
              else if (POS_DA_MENSAGEM<2)
                flash.msg[11][249] = CALCULO*10+usb_gBuf-48;//MENSAGEM EXTRAS
           }
        }
        if(!file_rw_cnt)
        {
          //ERRO_USB=1;
          //FDL_LoadControl();
          break;
        }
      }
      extern uint8_t msg_edicao[240];
      Sys_DisplayFlags = 0;
      Sys_DisplayFlags |= NeedToChangeActualMessage;
      LimpaTela();
      Sys_TypeOfMsgOnDisplay = OtherMessage;

     // uint32_t i = 7;
      //matriz temporaria pra receber essa encrenca
      msg_edicao[0] = 'U';
      msg_edicao[1] = 'S';
      msg_edicao[2] = 'B';
      msg_edicao[3] = ' ';
      msg_edicao[4] = 'O';
      msg_edicao[5] = 'K';
      flash.msg[0][249] |= 0x02;
      flash.msg[1][249] |= 0x02;
      flash.msg[2][249] |= 0x02;
      flash.msg[3][249] |= 0x02;
      flash.msg[4][249] |= 0x02;
      flash.msg[5][249] |= 0x02;
      flash.msg[6][249] |= 0x02;
      flash.msg[7][249] |= 0x02;
      flash.msg[8][249] |= 0x02;
      flash.msg[9][249] |= 0x02;
      flash.msg[10][249] |= 0x02;
      flash.msg[11][249] |= 0x02;

      flash.msg[0][252] = tempTacL;
      flash.msg[0][251] = tempTacH;
        //SE ESTIVER HABILITADA
      //for(uint32_t i = 1;i <= 12;i++)
     // {
      //  Sys_ConvertASCIItoMatriz(i);
      //}
      //======================================================================//
      /* Close the file object. */
      res = R_tfat_f_close(&file);
      if( res != TFAT_FR_OK )
      {
      }
      /* Check demo status. */
      if( usb_shmsc_demo_state != 0 )
      {
        /* Message next time we come back here is.. */
        usb_ghmsc_SmpAplProcess = USER_SW_PRESS2;
      }
      //      else
      //      {
      //        /* Message next time we come back here is.. */
      //        /* == 0 -> read loop. */
      //      }
      if( file_err != USB_E_ERROR )
      {
        usb_hmsc_SampleAplSpecifiedPath(mes);

        for(uint32_t i = 6;i != MENSAGEM_MAX_TAMANHO;i++)
        {
          msg.msg[i] = 0;
        }
        FDL_WriteControl();
      }
    }
    break;

  case USB_APL_CLR:
    break;

  default:
    break;
  }
  /* Release message memory from pool. */
  err = R_USB_REL_BLK(mess->flag);
  if( err != USB_E_OK )
  {
  }
}   /* eof usb_hmsc_SampleAplTask() */

/******************************************************************************
Function Name   : usb_hmsc_SampleAplSpecifiedPath
Description     : Next Process Selector
Argument        : usb_utr_t *mess
Return          : none
******************************************************************************/
void usb_hmsc_SampleAplSpecifiedPath(usb_tskinfo_t *mess)
{
  R_USB_GET_SND(USB_HMSCSMP_MBX, usb_ghmsc_SmpAplProcess, &usb_cstd_dummy_function, (usb_strct_t)mess->keyword);
}   /* eof usb_hmsc_SampleAplSpecifiedPath() */

/******************************************************************************
End  Of File
******************************************************************************/
/******************************************************************************
Function Name   : usb_hmsc_device_state
Description     : Open / Close
Arguments       : uint16_t data             : Device address
: uint16_t state            : Device state
Return value    : none
******************************************************************************/
USB_STATIC void usb_hmsc_device_state(uint16_t data, uint16_t state)
{
  switch( state )
  {
  case USB_STS_DETACH:
    usb_hmsc_DriveClose( data, 0 );   /* Callback when device is detached. */
    break;
  case USB_STS_ATTACH:
    break;
  case USB_STS_DEFAULT:
    R_usb_hmsc_Initialized();
    break;
  case USB_STS_CONFIGURED:
    usb_hmsc_DriveOpen( data, 0 );    /* Callback when device is configured. */
    break;
  case USB_STS_SUSPEND:
    break;
  case USB_STS_RESUME:
    break;
  case USB_STS_WAKEUP:
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

}   /* eof usb_hsmpl_device_state() */
