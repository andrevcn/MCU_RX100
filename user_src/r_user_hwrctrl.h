/*============================================================================*/
/*                                                                            */
/*      PROJECT: Painel Eletrônico VEHTEC                                     */
/*      FILE NAME: r_user_hwrctrl.h                                           */
/*      AUTHOR: Alison Luan Nunes de Souza                                    */
/*                                                                            */
/*      MODIF. DATE:                                                          */
/*              Rev.00: Abertura do projeto - 26/09/2013                      */
/*============================================================================*/
#ifndef __R_USER_HWCTRL_H
#define __R_USER_HWCTRL_H




/* PIN DESCRIPTION--------------------------------------------------------------------------------*/
#define         ADC_MCU_MSG1                   ADCHANNEL1
#define         ADC_MCU_MSG2                   ADCHANNEL2
#define         ADC_MCU_MSG3                   ADCHANNEL6
#define         ADC_MCU_MSG4                   ADCHANNEL8
#define         ADC_MCU_MSG5                   ADCHANNEL9
#define         ADC_MCU_SENSOR                 ADCHANNEL0
#define         ADC_LIGHT_SEN                  ADCHANNEL10
#define         ADC_TEMP_SEN                   ADCHANNEL11



/* FUNCTIONS PROTOTYPES---------------------------------------------------------------------------*/
uint16_t Delay_mS(uint16_t* value, uint16_t time);
void Sys_ConvertASCIItoMatriz(uint32_t MsgNumber);
void Sys_Maintenance(void);
uint8_t Sys_GetMessagePointerNext(void);
uint8_t Input_Read_CfgLevel(uint8_t msg_id);
uint8_t isMsg_Enable(uint8_t msg_id);
void ControleRTC(void);
void CalculoVelocidade(void);
void HoraPiscando(void);
void EntradaPosIgnicao(void);
void AlimentaWTD(void);
void ControleTemperaturaInterna(void);
void ControleTemperaturaExterna(void);
void ButtonHourUp(void);
void ButtonHourDown(void);
void LimpaTela(void);
void ZeraEfeito(void);
void verificaEntradas(unsigned char input);
void Display_MainTask(void);
void Display_Orientacao(void);
void Dsiplay_NormalMessage(void);
void Display_EditMessage(void);
void Display_ModoTeste(void);
void Display_OtherMessage(void);
void Display_SetRTC(void);
void Display_SetFatorK(void);
void Efeito_PacMan(void);
void Efeito_Matricial(void);
void Efeito_Dissolver(void);
void Efeito_SobrePor(void);
void Efeito_Laser(void);
void Efeito_Revelar(void);
void Efeito_RevelarInvertido(void);
void Efeito_Quicando(void);
void Efeito_Estatico(void);
void Efeito_Scroll_Right(void);
void Efeito_Scroll_left(void);
void Efeito_Scroll_Up(void);
void Efeito_Scroll_Down(void);
void Display_ProcessaEfeito(void);


/* TYPEDEFS---------------------------------------------------------------------------------------*/
typedef union{
  uint8_t FLASH_VECTOR[3072];//enche a página da memória
  struct {
    uint8_t msg[12][256];
  };
}Flash;
extern Flash flash;

typedef union{
  uint8_t MAIN_VECTOR[256];//carrega tudo pra ram (talvez não precise desse flash_vector
  struct{
    uint8_t msg[240];
    uint8_t cfg[5];
    uint8_t orientacao;
    uint8_t entrada;
    uint8_t repeticoes;
    uint8_t prioridade;
    uint8_t extras;//1 bit pra habilitar cada uma...
    uint8_t acionamento;
    uint16_t FatorK;
    uint8_t reserva6;
    uint8_t reserva7;
    uint8_t reserva8;
  };
}memoria;
extern memoria msg;

typedef struct{
  uint8_t normal;
  uint8_t extra;
  uint8_t editada;
}contador;
extern contador counter;


#endif
