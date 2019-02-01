/*=================================================================================================
      PROJECT	: Painel Eletrônico VEHTEC
      FILE NAME	: r_user_hwctrl.c
      AUTHOR	: Alison Luan Nunes de Souza
      DATE		: Rev.00: Abertura do projeto - 26/09/2013
==================================================================================================*/



/* INCLUDES---------------------------------------------------------------------------------------*/
#include "r_cg_macrodriver.h"
#include "r_cg_s12adb.h"
#include "r_user_includes.h"
#include "r_cg_userdefine.h"
#include "Fonts16.h"
#include "r_user_intempsensor.h"
#include "r_user_extempsensor.h"
#include "stdlib.h"
#include "r_user_hwrctrl.h"
#include "Entradas.h"
#include "r_cg_main.h"



/* VARIABLES EXTERNAS-----------------------------------------------------------------------------*/
extern uint8_t 	Sys_LedMatrixBuffer[LED_MATRIZ_MAX_COLUMNS]; /* Matriz Virtual de 8 linhas de 32 colunas cada para servir de buffer*/
extern uint8_t 	trocou_parametro;
extern uint8_t 	efeito_matricial;
extern uint8_t 	Sys_MatrixTempEffect2[LED_MATRIZ_MAX_COLUMNS];
extern uint8_t 	msg_edicao[240];
extern uint32_t Sys_DisplayFlags; 			/* Flags de controle do display */
extern uint32_t Sys_ActualDisplayMessage; 	/* Carrega o valor da mensagem atual sendo mostrada (0 -> nenhuma. 1-8 -> mensagens)*/
extern uint32_t Sys_MaxRefreshCount; 		/* Abriga o  número máximo de iterações para troca de dados na tela */
extern uint8_t 	Sys_PWMlatchEn;
extern uint32_t Sys_DisplayFlags; 			/* Flags de controle do display */
extern uint32_t Sys_ActualDisplayMessage; 	/* Carrega o valor da mensagem atual sendo mostrada (0 -> nenhuma. 1-8 -> mensagens)*/
extern uint32_t Sys_MaxRefreshCount; 		/* Abriga o  número máximo de iterações para troca de dados na tela */
extern uint8_t 	Sys_PWMlatchEn;
extern uint16_t Sys_FunctionSelected;


/* VARIABLES--------------------------------------------------------------------------------------*/
uint8_t 		Sys_MatrixTempEffect1[LED_MATRIZ_MAX_COLUMNS];
uint8_t 		Inversora[LED_MATRIZ_MAX_COLUMNS];
uint32_t 		Sys_MessageLoadControlPointer = 0;
uint8_t 		Sys_LedMatrixBufferVirada[LED_MATRIZ_MAX_COLUMNS];
uint8_t 		msg_edicao_temp[240];
uint8_t 		EstadoTeste=0;
uint16_t 		Sys_HoraEmMinutos = 60;
uint16_t 		Sys_Horas = 1;
uint16_t 		Sys_Minutos = 0;
uint16_t 		Sys_HoraA = 0;
uint16_t 		Sys_HoraB = 1;
uint16_t 		Sys_MinutoA = 0;
uint16_t 		Sys_MinutoB = 0;
uint8_t 		TemChicote = 0;
uint8_t 		DetectouChicote = 0;
uint8_t 		MAT_MSG[MENSAGEM_MAX_TAMANHO*10]; /* Matriz onde abrigará o valor já tratado */
uint8_t 		flag_efeito=0;
uint16_t 		x_efeito=0;
uint16_t 		y_efeito=0;
uint16_t 		z_efeito=0;
uint16_t 		w_efeito=1;
uint8_t 		Sys_TachU;
uint8_t 		Sys_TachD;
uint8_t 		Sys_TachC;
uint16_t		Sys_GeneralFlags;
uint16_t        Sys_TickTimer;
uint16_t        Sys_InternalTemperatureBuffer;
uint16_t        Sys_ExternalTemperatureBuffer;
uint16_t        Sys_LightBuffer;
uint16_t        Sys_StopFlowCounter;
uint16_t        Sys_StopFlowCounterLimit;
uint16_t        Sys_StopFlowInit;
uint16_t        Sys_TypeOfMsgOnDisplay;
uint16_t        Sys_EditPointMsg;
uint8_t         Sys_MessagePointer = 0;
uint16_t        Sys_InfoDelay = 0;
uint16_t        Sys_InfoDelayTime;
uint8_t         AjusteHora;
uint8_t         AjusteMinuto;
uint16_t        Tamanho;
uint16_t        Tamanho_edit;
uint8_t         pisca_edicao=0;
uint16_t        tempo_pisca=0;
uint8_t         piscaHora = 0;
uint8_t   		PrioridadeAtual = 0;
uint8_t   		PrioridadeAcionada = 0;
uint8_t   		EntradaAtual = 0;
uint8_t   		EntradaAcionada = 0;
uint8_t   		MensagemEntrada = 0;
uint32_t		Repeticoes = 0;
uint8_t			Entrada[5];
uint8_t			AcionamentoEntrada[5];
uint8_t			EntradaVariou[5];
uint8_t			EntradaFlag[5];
uint32_t    	ContagemMsg[5];
uint32_t    	VelocidadeTac = 0;
uint32_t    	FatorK;
uint32_t   		ContadorPulsosTac = 0;
uint16_t		Sys_TempA_int;
uint16_t		Sys_TempB_int;
uint16_t		Sys_TempC_int;
uint16_t		Sys_TempA_ext;
uint16_t		Sys_TempB_ext;
uint16_t		Sys_TempC_ext;
uint16_t		Sys_RTC=0;
uint8_t		    Sys_InputTurnOnProductCnt = 0;
uint8_t			Sys_InputTurnOnProduct = 1;
uint32_t		Sys_MessageLoadControlPointerMax = LED_MATRIZ_MAX_COLUMNS;
uint8_t			PiscaTach = 0;
uint8_t			ToogleTach = 0;


/*=================================================================================================

      FUNCTION: Delay_mS
      DESCRIPTION: Create a free software flow base of time
      PARAMETERS:
              uint16_t* value
                      pointer to dummy register that holds first dot time
              uint16_t time
                      specify the time delay that you want to create
      RETURN:
              uint16_t
                      0 - time delay do not reached
                      1 - time delay reached

==================================================================================================*/
uint16_t Delay_mS(uint16_t* value, uint16_t time)
{
  uint16_t rc = 0;

  if(*value == 0)
  {
    /* First time iteration - loads Sys_TickTimer value*/
    *value = Sys_TickTimer;
  }
  else
  {
    if(Sys_TickTimer >= *value)
    {
      /* if Sys_TickTimer are higher than stored value, the operation is normal */
      if((Sys_TickTimer - *value) >= time)
      {
        *value = 0;
        rc = 1; /* time delay reached! */
      }
    }
    else
    {
      /* in this case, Sys_TickTimer overflowed, so we need to count the time after the first read */
      /* to the overflow value plus the actual MTU0.TCNT value */
      if(((0xFFFF - *value)+Sys_TickTimer) >= time)
      {
        *value = 0;
        rc = 1; /* time delay reached! */
      }
    }
  }
  return rc;
}

/*==================================================================================================
      FUNCTION		: Sys_ConvertASCIItoMatriz
      DESCRIPTION	: Converte a mensagem armazenada em ASCII em forma de matriz
      PARAMETERS	: uint32_t MsgNumber
                      Número da mensagem (0-7) a ser transformada em matriz
      RETURN		: nothing.
==================================================================================================*/
void Sys_ConvertASCIItoMatriz(uint32_t MsgNumber)//passar se é msg.msg ou msg_edicao
{
	const uint16_t* font_pointer_buffer;//endereço inicial das fontes
	const uint16_t* font_pointer;
	uint32_t address;
  	uint8_t raw_char;
  	uint32_t msg_p = 0;
  	uint32_t i;
  	uint32_t j;
  	uint8_t z;


	if (Sys_TypeOfMsgOnDisplay != EditMessage)
	  pisca_edicao=0;

  	if (!msg.orientacao || msg.orientacao==1)
  	{
    	font_pointer_buffer = Normal8X5;
    	z = 6;
  	}
  	else
 	{
    	font_pointer_buffer = Virado8X5;
    	z = 10;
  	}

	for(i = 0;i < MENSAGEM_MAX_TAMANHO; i++) /* Varre todas mensagem em ASCII armazenada */
  	{
    	if (Sys_TypeOfMsgOnDisplay)
      		raw_char = msg_edicao_temp[i];
    	else
			raw_char = msg.msg[i]; /* Carrega o caracter atual */

		if(raw_char == 0) /* Fim de frase */
    	{
      		if(msg_p)
      		{
        		msg_p -= 1;
      		}
      		break;
    	}
    	else
    	{
      		for(j = 0;j < z;j++) /* Varre o caracter carregado. No máximo 6 colunas */
      		{
        		address = (raw_char-0x20)*z+j;//MUDAR ESSE LANCE AQUI PARA CARREGAR CARACTERES MAIORES
        		font_pointer = (font_pointer_buffer+address);
        		MAT_MSG[msg_p] = *font_pointer; /* Coloca a coluna na matriz final de dados */

				if (i<Sys_EditPointMsg)
        		{
          			Tamanho_edit++;
        		}

				msg_p++; /* Incrementa o ponteiro de msg */

				if(*font_pointer & 0x8000) /* Testa se existe o caracter delimitador na mensagem */
        		{
          			MAT_MSG[msg_p] = 0x00; /* Inserção de um espaço entre caracteres */
          			msg_p++;
          			if (i<Sys_EditPointMsg)
          			{
            			Tamanho_edit++;
          			}
          			break;
        		}
      		}
    	}
  	}

  	if (!pisca_edicao)
    	Tamanho = msg_p; /* Carrega o tamanho da mensagem menos 1 (retira o último espaço colocado) */

  	for(;msg_p < (MENSAGEM_MAX_TAMANHO * 10); msg_p++) /* Varre todas mensagem armazenada na matriz */
  	{
	    MAT_MSG[msg_p] = 0;
  	}
}

/*==================================================================================================
      FUNCTION		: verificaEntradas
      DESCRIPTION	:
      PARAMETERS	: uint8_t input
      RETURN		: nothing.
==================================================================================================*/
void verificaEntradas(uint8_t input)
{
	if (flash.msg[input][241]) //MSG HABILITADA???????
	{
		PrioridadeAcionada = flash.msg[input][248];//verifica a prioridade
		if (PrioridadeAcionada > PrioridadeAtual)
		{
			Sys_MessagePointer = input;
			OpenMsg(Sys_MessagePointer);
			ZeraEfeito();
			PrioridadeAtual = PrioridadeAcionada;
			Repeticoes = flash.msg[input][247];
			Sys_ActualDisplayMessage = Sys_MessagePointer;
			Sys_MaxRefreshCount      = msg.cfg[MSG_INFO_VEL_DE_TROCA];
			Sys_StopFlowCounterLimit = msg.cfg[MSG_INFO_QUADROS_PARADOS];
			trocou_parametro = 1;
			Sys_DisplayFlags |= NeedToChangeActualMessage;
		}
	}
}

/*==================================================================================================
      FUNCTION		: Sys_RotEstaticaEffect
      DESCRIPTION	: Controla a atualização estática da matriz de LEDs
      PARAMETERS	: uint32_t mode
                      0 - não apaga a tela onde não há dados
                      1 - apaga a tela onde não há dados
      RETURN		: nothing.
==================================================================================================*/
void Sys_RotEstaticaEffect(uint32_t mode)//mode nao eh usado
{
	for(uint32_t i=0;i<LED_MATRIZ_MAX_COLUMNS;i++) /* Varre todo tamanho do buffer de tela */
	{
		Sys_LedMatrixBuffer[i] = MAT_MSG[i];
	}
}

/*==================================================================================================
      FUNCTION		: ZeraEfeito
      DESCRIPTION	:
      PARAMETERS	: nothing
      RETURN		: nothing.
==================================================================================================*/
void ZeraEfeito(void)
{
	Sys_DisplayFlags = 0;
	Sys_DisplayFlags |= NeedToChangeActualMessage;
	efeito_matricial = 0;
	Sys_StopFlowCounter = 0;

	for (uint8_t i=0;i<240;i++)
	{
		msg.msg[i]=0;
		msg_edicao[i]=0;
		msg_edicao_temp[i]=0;
	}

	Sys_ConvertASCIItoMatriz(0);
	Sys_MessageLoadControlPointer = 0;
	LimpaTela();
	flag_efeito=0;
	x_efeito=0;
	y_efeito=0;
	z_efeito=0;
	w_efeito=1;
	msg.cfg[MSG_INFO_EFEITO] = 0;
}

/*==================================================================================================
      FUNCTION		: LimpaTela
      DESCRIPTION	:
      PARAMETERS	: nothing
      RETURN		: nothing.
==================================================================================================*/
void LimpaTela(void)
{
	efeito_matricial=0;
	for(uint8_t i=0;i<LED_MATRIZ_MAX_COLUMNS;i++)
	{
		Sys_LedMatrixBuffer[i] = 0;
		Sys_MatrixTempEffect1[i] = 0;
		Sys_MatrixTempEffect2[i] = 0;
		Sys_LedMatrixBufferVirada[i] = 0;
	}
}

/*==================================================================================================
      FUNCTION		: Sys_Maintenance
      DESCRIPTION	: Controla o fluxo de todas funções do software
      PARAMETERS	: nothing.
      RETURN		: nothing.
==================================================================================================*/
void Sys_Maintenance(void)//FUNÇÃO PRINCIPAL
{
	AlimentaWTD();
	EntradaPosIgnicao();
	ButtonHourUp();
	ButtonHourDown();
	ControleRTC();
	CalculoVelocidade();
	HoraPiscando();
	ControleTemperaturaInterna();
    Display_MainTask();
    Display_ModoTeste();
	Display_Orientacao();
}

/*=================================================================================================*
*      FUNCTION:     Display_MainTask                                                              *
*      DESCRIPTION:                                                                                *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
void Display_MainTask(void)
{
	/* Atualização do buffer na tela */
	if(!Sys_InputTurnOnProduct && TemChicote)
	{
		if(Sys_TypeOfMsgOnDisplay != OtherMessage)
		{
			ZeraEfeito();
			Sys_MessagePointer = Sys_GetMessagePointerNext(); //Verifica qual a proxima msg que deve mostrar, dependendo das confg.
		}
	}
	else
	{
		if(Sys_DisplayFlags & CanUpdateBuffer)
		{
			Sys_DisplayFlags &= ~CanUpdateBuffer;

			if(Sys_TypeOfMsgOnDisplay == NormalMessage || Sys_TypeOfMsgOnDisplay == EditMessage || Sys_TypeOfMsgOnDisplay == SetRTC || Sys_TypeOfMsgOnDisplay == SetFatorK)
			{
				/* Verifica o efeito a ser utilizado */
				if(Sys_TypeOfMsgOnDisplay == SetRTC || Sys_TypeOfMsgOnDisplay == EditMessage)
				{
					msg.cfg[MSG_INFO_EFEITO] = 0;
				}

				Display_ProcessaEfeito();

				if(Sys_DisplayFlags & NeedToNormalFlow)//FICA AQUI DURANTE O EFEITO
				{
					//VAI PEGANDO CADA COLUNA DA MSG...
					Sys_MessageLoadControlPointer++;

					if(Sys_MessageLoadControlPointer >= Sys_MessageLoadControlPointerMax)
					{
						Sys_MessageLoadControlPointer = 0;
						Sys_DisplayFlags &= ~NeedToNormalFlow;
						Sys_DisplayFlags |= NeedToStopFlow;
					}
				}

				if(Sys_DisplayFlags & NeedToStopFlow)
				{
					Sys_StopFlowCounter++;

					if (Sys_MessagePointer<12)
					{
						Sys_MaxRefreshCount = 3;
					}

					if(Sys_StopFlowCounter > Sys_StopFlowCounterLimit)//definido na matriz de configuração
					{
						flag_efeito = 0;
						x_efeito = 0;
						y_efeito = 0;
						w_efeito = 1;
						z_efeito = 0;
						Sys_StopFlowCounter = 0;
						Sys_DisplayFlags &= ~NeedToStopFlow;
						Sys_DisplayFlags |= NeedToChangeActualMessage;
					}
				}

				if(Sys_DisplayFlags & NeedToFlowWithNothing)
				{
					Sys_DisplayFlags &= ~NeedToFlowWithNothing;
					Sys_DisplayFlags |= NeedToChangeActualMessage;
				}
			}
		}
	}

	if(Sys_DisplayFlags & NeedToChangeActualMessage)
	{
		switch(Sys_TypeOfMsgOnDisplay){
		case NormalMessage:		Dsiplay_NormalMessage();	break;
		case EditMessage:		Display_EditMessage();		break;
		case SetRTC:			Display_SetRTC();			break;
		case SetFatorK:			Display_SetFatorK();		break;
		case OtherMessage:		Display_OtherMessage();		break;
		}
	}
}

/*=================================================================================================*
*      FUNCTION:     Display_ProcessaEfeito                                                        *
*      DESCRIPTION:                                                                                *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
void Display_ProcessaEfeito(void)
{
	switch(msg.cfg[MSG_INFO_EFEITO]){
	case ROT_EFEITO_PACMAN:
		Efeito_PacMan();
		break;
	case ROT_EFEITO_MATRICIAL:
		Efeito_Matricial();
		break;
	case ROT_EFEITO_DISSOLVER:
		Efeito_Dissolver();
		break;
	case ROT_EFEITO_SOBREPOR:
		Efeito_SobrePor();
		break;
	case ROT_EFEITO_LASER:
		Efeito_Laser();
		break;
	case ROT_EFEITO_REVELAR:
		Efeito_Revelar();
		break;
	case ROT_EFEITO_REVELAR_INV:
		Efeito_RevelarInvertido();
		break;
	case ROT_EFEITO_QUICANDO:
		Efeito_Quicando();
		break;
	case ROT_EFEITO_ESTATICA:
		Efeito_Estatico();
		break;
	case ROT_EFEITO_DIREITA:
		Efeito_Scroll_Right();
		break;
	case ROT_EFEITO_ESQUERDA:
	case EMPURRAR_ESQUERDA:
	case EFEITO_SCROLL:
		Efeito_Scroll_left();
		break;
	case ROT_EFEITO_CIMA:
	case EMPURRAR_CIMA:
		Efeito_Scroll_Up();
		break;
	case ROT_EFEITO_BAIXO:
	case EMPURRAR_BAIXO:
		Efeito_Scroll_Down();
		break;
	}
}

/*=================================================================================================*
*      FUNCTION:     Display_Orientacao                                                            *
*      DESCRIPTION:                                                                                *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
void Display_Orientacao(void)
{
	unsigned short cont_i,cont_d;
	unsigned char Invert;
	unsigned char vByte;

	if (msg.orientacao==1||msg.orientacao==3)
	{
		cont_i = 0;
		cont_d = LED_MATRIZ_MAX_COLUMNS;

		for (uint8_t h=0;h<LED_MATRIZ_MAX_COLUMNS;h++)
		{
			if (efeito_matricial)
				Inversora[h] = Sys_LedMatrixBuffer[h]|Sys_MatrixTempEffect2[h];
			else
				Inversora[h] = Sys_LedMatrixBuffer[h];
		}

		do //compara bit a bit
		{
			cont_d--;
			vByte   =   (Inversora[cont_i]);
			Invert  = vByte & 0x80 ? 0x01 : 0;
			Invert |= vByte & 0x40 ? 0x02 : 0;
			Invert |= vByte & 0x20 ? 0x04 : 0;
			Invert |= vByte & 0x10 ? 0x08 : 0;
			Invert |= vByte & 0x08 ? 0x10 : 0;
			Invert |= vByte & 0x04 ? 0x20 : 0;
			Invert |= vByte & 0x02 ? 0x40 : 0;
			Invert |= vByte & 0x01 ? 0x80 : 0;
			Sys_LedMatrixBufferVirada[cont_d]  = Invert;
			cont_i++;
		}while(cont_d);
	}
}

/*=================================================================================================*
*      FUNCTION:     Display_SetFatorK                                                             *
*      DESCRIPTION:                                                                                *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
void Display_SetFatorK(void)
{
	PiscaTach++;
	if (PiscaTach > 250)
	{
		PiscaTach=0;
		ToogleTach =  !ToogleTach;
	}

	if (ToogleTach)
	{
		msg_edicao[0]=Sys_TachC+'0';
		msg_edicao[1]=Sys_TachD+'0';
		msg_edicao[2]=Sys_TachU+'0';
	}
	else
	{
		if (Sys_EditPointMsg==0)
			msg_edicao[0]='_';
		else
			msg_edicao[0]=Sys_TachC+'0';

		if (Sys_EditPointMsg==1)
			msg_edicao[1]='_';
		else
			msg_edicao[1]=Sys_TachD+'0';

		if (Sys_EditPointMsg==2)
			msg_edicao[2]='_';
		else
			msg_edicao[2]=Sys_TachU+'0';
	}

	msg_edicao[3]=0;
	FatorK = Sys_TachC*100+Sys_TachD*10+Sys_TachU;

	for (uint8_t i=0;i<240;i++)
	{
		msg_edicao_temp[i] = msg_edicao[i];
	}

	Sys_ConvertASCIItoMatriz(Sys_MessagePointer);
	Sys_RotEstaticaEffect(0);
}

/*=================================================================================================*
*      FUNCTION:     Display_SetRTC                                                                *
*      DESCRIPTION:                                                                                *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
void Display_SetRTC(void)
{
	Sys_HoraA = Sys_Horas/10;
	Sys_HoraB = (Sys_Horas%10);
	Sys_MinutoA = Sys_Minutos/10;
	Sys_MinutoB = Sys_Minutos%10;
	msg_edicao[0]=Sys_HoraA+'0';
	msg_edicao[1]=Sys_HoraB+'0';
	msg_edicao[2]=':';
	msg_edicao[3]=Sys_MinutoA+'0';
	msg_edicao[4]=Sys_MinutoB+'0';
	msg_edicao[5]=0;
	for (uint8_t i=0;i<240;i++)
	{
		msg_edicao_temp[i] = msg_edicao[i];
	}
	Sys_ConvertASCIItoMatriz(Sys_MessagePointer);
	Sys_RotEstaticaEffect(0);
}

/*=================================================================================================*
*      FUNCTION:     Dsiplay_NormalMessage                                                         *
*      DESCRIPTION:                                                                                *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
void Dsiplay_NormalMessage(void)
{
	Sys_DisplayFlags |= NeedToNormalFlow;//CHAMA PARA FAZER O EFEITO DA MSG...
	Sys_DisplayFlags &= ~NeedToChangeActualMessage; /* Limpa flag de necessidade de carregamento de outra mensagem */
	Sys_DisplayFlags &= ~(NeedToStopFlow | NeedToFlowWithNothing);//GARANTE QUE NÃO ESTEJA NOS ESTADOS DE TELA PARADA OU SEM NADA NA TELA
	flag_efeito = 0;

	if (trocou_parametro == 0)
	{
		Sys_MessagePointer = Sys_GetMessagePointerNext(); //Verifica qual a proxima msg que deve mostrar, dependendo das confg.
	}
	else
	{
		trocou_parametro = 0;
	}

	if (Sys_MessagePointer<12)//MENSAGENS NORMAIS
	{
		for (uint16_t i=0;i<256;i++)
		{
			msg.MAIN_VECTOR[i] = flash.msg[Sys_MessagePointer][i];
		}
	}
	else
	{
		Sys_MessageLoadControlPointerMax=0;
		for (uint16_t i=0;i<240;i++)
		{
			msg.msg[i]=0;
		}

		if((flash.msg[11][249] & 0x01) && Sys_MessagePointer==12) //HORARIO
		{
			msg.cfg[MSG_INFO_EFEITO] = ROT_EFEITO_ESTATICA;
			Sys_MaxRefreshCount = 60;//DEZ SEGUNDOS EXIBINDO A HORA
			Sys_ActualDisplayMessage = Sys_MessagePointer;
			msg.msg[0]=Sys_HoraA+ '0';
			msg.msg[1]=Sys_HoraB+ '0';
			msg.msg[2]=':';
			msg.msg[3]=Sys_MinutoA+ '0';
			msg.msg[4]=Sys_MinutoB+ '0';
			Sys_ConvertASCIItoMatriz(Sys_MessagePointer);
		}

		if ((flash.msg[11][249] & 0x02) && Sys_MessagePointer==13) //VELOCIDADE
		{
			msg.cfg[MSG_INFO_EFEITO] = ROT_EFEITO_ESTATICA;
			Sys_MaxRefreshCount = 60;
			unsigned char Dezena, Centena, Unidade;
			Centena = VelocidadeTac/100;
			Dezena = (VelocidadeTac-Centena*100)/10;
			Unidade = VelocidadeTac-Centena*100 - Dezena*10;
			Sys_ActualDisplayMessage = Sys_MessagePointer;

			if (VelocidadeTac>=100)
			{
				Unidade = 9;
				Dezena = 9;
			}
			msg.msg[0]=Dezena + '0';
			msg.msg[1]=Unidade + '0';
			msg.msg[2]='k';
			msg.msg[3]='m';
			msg.msg[4]='h';
			Sys_ConvertASCIItoMatriz(Sys_MessagePointer);
		}

		if ((flash.msg[11][249] & 0x04) && Sys_MessagePointer==14)
		{
			Sys_ActualDisplayMessage = Sys_MessagePointer;
			Sys_MaxRefreshCount = 25;
			msg.cfg[MSG_INFO_EFEITO] = EFEITO_SCROLL;
			msg.msg[0]='T';
			msg.msg[1]='-';
			msg.msg[2]='I';
			msg.msg[3]='N';
			msg.msg[4]=' ';
			msg.msg[5]=Sys_TempA_int+'0';
			msg.msg[6]=Sys_TempB_int+'0';
			msg.msg[7]='.';
			msg.msg[8]=Sys_TempC_int+'0';
			msg.msg[9]='°';
			msg.msg[10]='C';
			Sys_ConvertASCIItoMatriz(Sys_MessagePointer);
		}

		if ((flash.msg[11][249] & 0x08) && Sys_MessagePointer==15 && Sys_ExternalTemperatureBuffer > 100)
		{
			Sys_ActualDisplayMessage = Sys_MessagePointer;
			Sys_MaxRefreshCount = 25;
			msg.cfg[MSG_INFO_EFEITO] = EFEITO_SCROLL;
			msg.msg[0]='T';
			msg.msg[1]='-';
			msg.msg[2]='E';
			msg.msg[3]='X';
			msg.msg[4]=' ';
			msg.msg[5]=Sys_TempA_ext+'0';
			msg.msg[6]=Sys_TempB_ext+'0';
			msg.msg[7]='.';
			msg.msg[8]=Sys_TempC_ext+'0';
			msg.msg[9]='°';
			msg.msg[10]='C';
			Sys_ConvertASCIItoMatriz(Sys_MessagePointer);
		}
	}
	//OS LOAD DA MEMORIA VAO AQUI

	//VAI PUXANDO AS MSGS DA MEMÓRIA AQUI... E DEPOIS VAI PRO LAÇO CONDICIONAL
	if(msg.cfg[MSG_INFO_HABILITADA] && Sys_MessagePointer<12) /* Caso a mensagem esteja habilitada, usa ela */
	{
		Sys_ConvertASCIItoMatriz(Sys_MessagePointer);
		Sys_ActualDisplayMessage = Sys_MessagePointer;
		Sys_MaxRefreshCount = msg.cfg[MSG_INFO_VEL_DE_TROCA]; /* Carrega a nova velocidade de atualização referente a mensagem */
		Sys_StopFlowCounterLimit = msg.cfg[MSG_INFO_QUADROS_PARADOS];

		/* Pré-load das mensagens */
		if (Tamanho>(LED_MATRIZ_MAX_COLUMNS) && msg.cfg[MSG_INFO_EFEITO] != ROT_EFEITO_PACMAN) //caso o tamanho da mensagem seja maior do que a tela, força o efeito scroll
		{
			msg.cfg[MSG_INFO_EFEITO] = ROT_EFEITO_ESQUERDA;
			Sys_StopFlowCounterLimit = 0;
		}

		if (msg.cfg[MSG_INFO_EFEITO] == ROT_EFEITO_PACMAN || msg.cfg[MSG_INFO_EFEITO] == EFEITO_SCROLL)
			Sys_StopFlowCounterLimit = 0;

		if (msg.cfg[MSG_INFO_EFEITO]==ROT_EFEITO_LASER||msg.cfg[MSG_INFO_EFEITO]==ROT_EFEITO_MATRICIAL)
			Sys_MaxRefreshCount = 4;

		switch(msg.cfg[MSG_INFO_EFEITO])
		{
		case ROT_EFEITO_ESTATICA: /* Mensagem estática */
			Sys_MessageLoadControlPointerMax = 5;
			break;

		case ROT_EFEITO_DIREITA: /* Mensagem rotacionada a direita */
			Sys_MessageLoadControlPointerMax = Tamanho;
			break;

		case ROT_EFEITO_ESQUERDA: /* Mensagem rotacionada a esquerda */
		case EMPURRAR_ESQUERDA:

			if (Tamanho>LED_MATRIZ_MAX_COLUMNS)
				Sys_MessageLoadControlPointerMax = Tamanho+LED_MATRIZ_MAX_COLUMNS;
			else
				Sys_MessageLoadControlPointerMax = LED_MATRIZ_MAX_COLUMNS;
			break;

		case EFEITO_SCROLL:
			Sys_MessageLoadControlPointerMax = Tamanho+LED_MATRIZ_MAX_COLUMNS;
			break;

		case ROT_EFEITO_CIMA: /* Mensagem rotacionada pra cima */
		case ROT_EFEITO_BAIXO: /* Mensagem rotacionada pra baixo */
		case EMPURRAR_CIMA:
		case EMPURRAR_BAIXO:
			Sys_MessageLoadControlPointerMax = 8; /* Troca após 8 subidas */
			break;

		case ROT_EFEITO_QUICANDO:
			Sys_MessageLoadControlPointerMax = 14;
			break;

		case ROT_EFEITO_REVELAR:
		case ROT_EFEITO_REVELAR_INV:
		case ROT_EFEITO_SOBREPOR:
			Sys_MessageLoadControlPointerMax = LED_MATRIZ_MAX_COLUMNS;
			break;

		case ROT_EFEITO_MATRICIAL:
			Sys_MessageLoadControlPointerMax = LED_MATRIZ_MAX_COLUMNS*8;
			break;
		case ROT_EFEITO_LASER:
			Sys_MessageLoadControlPointerMax = (Tamanho)*8 + Tamanho;
			break;

		case ROT_EFEITO_DISSOLVER:
			Sys_MessageLoadControlPointerMax = 64;
			break;

		case ROT_EFEITO_PACMAN:
			Sys_MessageLoadControlPointerMax = Tamanho + LED_MATRIZ_MAX_COLUMNS;
			break;

		}

		if (msg.cfg[MSG_INFO_EFEITO] != ROT_EFEITO_ESTATICA &&
			msg.cfg[MSG_INFO_EFEITO] != EMPURRAR_CIMA &&
			msg.cfg[MSG_INFO_EFEITO] != EMPURRAR_BAIXO &&
			msg.cfg[MSG_INFO_EFEITO] != EMPURRAR_ESQUERDA)
		{
			LimpaTela();
		}
	}
	else if (Sys_MessagePointer > 11)
	{
		Sys_MessageLoadControlPointerMax = Tamanho + LED_MATRIZ_MAX_COLUMNS;

		if ((Sys_MessagePointer==12  && flash.msg[11][249] & 0X01) ||
			(Sys_MessagePointer==13  && flash.msg[11][249] & 0X02) ||
			(Sys_MessagePointer==14  && flash.msg[11][249] & 0X04) ||
			((Sys_MessagePointer==15 && flash.msg[11][249] & 0X08) && Sys_ExternalTemperatureBuffer > 100))
		{
			Sys_StopFlowCounterLimit = 0;

			if (msg.cfg[MSG_INFO_EFEITO] != ROT_EFEITO_ESTATICA &&
				msg.cfg[MSG_INFO_EFEITO] != EMPURRAR_CIMA &&
				msg.cfg[MSG_INFO_EFEITO] != EMPURRAR_BAIXO &&
				msg.cfg[MSG_INFO_EFEITO] != EMPURRAR_ESQUERDA)
			{
				LimpaTela();
			}
		}
		else
		{
			Sys_MaxRefreshCount = 0;
			Sys_StopFlowCounterLimit = 0;
			Sys_MessageLoadControlPointerMax = 0;
			Sys_DisplayFlags |= NeedToChangeActualMessage;
			Sys_StopFlowCounterLimit = 0;
		}
	}
	else
	{
		Sys_DisplayFlags |= NeedToChangeActualMessage;
		Sys_StopFlowCounterLimit = 0;
	}
}

/*=================================================================================================*
*      FUNCTION:     Display_EditMessage                                                           *
*      DESCRIPTION:                                                                                *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
void Display_EditMessage(void)
{
	Sys_StopFlowInit= Tamanho_edit - 25;
	for(uint32_t i=0;i<100;i++)
	{
		if(Tamanho_edit > 25)
			Sys_LedMatrixBuffer[i] = MAT_MSG[i+Sys_StopFlowInit];
		else
			Sys_LedMatrixBuffer[i] = MAT_MSG[i];
	}

	for (uint8_t i=0;i<240;i++)
	{
		if (!pisca_edicao)
		{
			msg_edicao_temp[i] = msg_edicao[i];
		}
		else
		{
			if (i==Sys_EditPointMsg)
				msg_edicao_temp[i] = '_';
			else
				msg_edicao_temp[i] =  msg_edicao[i];
		}
	}

	Tamanho_edit = 0;
	Sys_ConvertASCIItoMatriz(1);

	tempo_pisca++;
	if (tempo_pisca > 350)
	{
		tempo_pisca=0;
		pisca_edicao =  !pisca_edicao;
	}
}

/*=================================================================================================*
*      FUNCTION:     Display_OtherMessage                                                          *
*      DESCRIPTION:                                                                                *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
void Display_OtherMessage(void)
{
	tempo_pisca++;
	for (uint8_t i=0;i<240;i++)
	{
		msg_edicao_temp[i] = msg_edicao[i];
	}

	Sys_ConvertASCIItoMatriz(1);
	Sys_RotEstaticaEffect(0);

	if (tempo_pisca > 1100)
	{
		for (uint8_t i=0;i<240;i++)
		{
			msg_edicao_temp[i] = 0;
		}
		Sys_ConvertASCIItoMatriz(1);
		if (!trocou_parametro)
		{
			Sys_MessagePointer = 0;
			ZeraEfeito();
		}
	}

	if (tempo_pisca>1200)
	{
		tempo_pisca=0;
		Sys_TypeOfMsgOnDisplay = NormalMessage;
		Sys_DisplayFlags &= ~NeedToNormalFlow;
		Sys_DisplayFlags &= ~NeedToStopFlow;
		Sys_DisplayFlags &= NeedToFlowWithNothing;
		Sys_DisplayFlags |= NeedToChangeActualMessage;
		ZeraEfeito();
	}
}

/*=================================================================================================*
*      FUNCTION:     Display_ModoTeste                                                             *
*      DESCRIPTION:                                                                                *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
void Display_ModoTeste(void)
{
	static uint16_t	Sys_DelayTesteAmbiente;

	if (Sys_TypeOfMsgOnDisplay == ModoTeste)
	{
		for (uint8_t i=0;i<240;i++)
		{
			msg_edicao_temp[i] = 0;
		}

		switch (EstadoTeste)
		{
			case 0:
			{
				for (uint8_t i=0;i<80;i++)
				{
					if (i%2)
						Sys_LedMatrixBuffer[i] = 255;
					else
						Sys_LedMatrixBuffer[i] = 0;
				}
				break;
			}

			case 1:
			{
				for (uint8_t i=0;i<LED_MATRIZ_MAX_COLUMNS;i++)
				{
					if (!(i%2))
						Sys_LedMatrixBuffer[i] = 255;
					else
						Sys_LedMatrixBuffer[i] = 0;
				}
				break;
			}

			case 2:
			{
				for (uint8_t i=0;i<LED_MATRIZ_MAX_COLUMNS;i++)
				{
					Sys_LedMatrixBuffer[i] = 0X55;
				}
				break;
			}

			case 3:
			{
				for (uint8_t i=0;i<LED_MATRIZ_MAX_COLUMNS;i++)
				{
					Sys_LedMatrixBuffer[i] = 0XAA;
				}
				break;
			}

			case 4:
			{
				for (uint8_t i=0;i<LED_MATRIZ_MAX_COLUMNS;i++)
				{
					Sys_LedMatrixBuffer[i] = 0XFF;
				}
				break;
			}

			case 5:
			{
				msg_edicao_temp[0]=Sys_TempA_int+'0';
				msg_edicao_temp[1]=Sys_TempB_int+'0';
				msg_edicao_temp[2]='.';
				msg_edicao_temp[3]=Sys_TempC_int+'0';
				msg_edicao_temp[4]='°';
				msg_edicao_temp[5]='C';
				Sys_ConvertASCIItoMatriz(1);
				Sys_RotEstaticaEffect(0);
				break;
			}

			case 6:
			{
				msg_edicao_temp[0]=Sys_TempA_ext+'0';
				msg_edicao_temp[1]=Sys_TempB_ext+'0';
				msg_edicao_temp[2]='.';
				msg_edicao_temp[3]=Sys_TempC_ext+'0';
				msg_edicao_temp[4]='°';
				msg_edicao_temp[5]='C';
				Sys_ConvertASCIItoMatriz(1);
				Sys_RotEstaticaEffect(0);
				break;
			}

			case 7:
			{

				if(!(PORT1.PIDR.BYTE & HOUR_UP))
				{
					msg_edicao_temp[0]='T';
					msg_edicao_temp[1]='C';
					msg_edicao_temp[2]='1';
					Sys_ConvertASCIItoMatriz(1);
					Sys_RotEstaticaEffect(0);
				}

				if(!(PORT2.PIDR.BYTE & HOUR_DOWN))
				{
					msg_edicao_temp[0]='T';
					msg_edicao_temp[1]='C';
					msg_edicao_temp[2]='2';
					Sys_ConvertASCIItoMatriz(1);
					Sys_RotEstaticaEffect(0);
				}
				break;
			}

			case 8: //TESTE DO BRILHO
			{
				static uint8_t flagAmbiente;
				static uint16_t vlr1,vlr2;
				if (!TemChicote)
				{
					if(Delay_mS(&Sys_DelayTesteAmbiente,800))
						flagAmbiente = !flagAmbiente;

					for (uint8_t i=0;i<80;i++)
					{
						if (flagAmbiente)
						{
							Sys_LedMatrixBuffer[i] = 0XFF;
							vlr1 = Sys_LightBuffer;
						}
						else
						{
							Sys_LedMatrixBuffer[i] = 0X00;
							vlr2 = Sys_LightBuffer;
						}

						if (vlr1 && vlr2)
						{
							if ((vlr1 - vlr2)>25)
							{
								Sys_TypeOfMsgOnDisplay = NormalMessage;
								EstadoTeste = 0;
							}
						}
					}
				}
				else
				{
					Sys_TypeOfMsgOnDisplay = NormalMessage;
					EstadoTeste = 0;
				}
				break;
			}
		}
	}
}

/*=================================================================================================*
*      FUNCTION:     ControleRTC                                                                   *
*      DESCRIPTION:                                                                                *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
void ControleRTC(void)
{
	static uint16_t Sys_DelayRelogio;
	if(Delay_mS(&Sys_DelayRelogio,59925) && Sys_TypeOfMsgOnDisplay != SetRTC)
	{
		Sys_HoraEmMinutos++;
		if(Sys_HoraEmMinutos > 779)
		{
			Sys_HoraEmMinutos = 60;
		}
		Sys_HoraA = Sys_HoraEmMinutos/600;
		Sys_HoraB = (Sys_HoraEmMinutos / 60) - (Sys_HoraA * 10);
		Sys_MinutoA = (Sys_HoraEmMinutos - ((Sys_HoraA * 600) + (Sys_HoraB * 60)))/10;
		Sys_MinutoB = (Sys_HoraEmMinutos - ((Sys_HoraA * 600) + (Sys_HoraB * 60) + (Sys_MinutoA * 10)));
	}
}

/*=================================================================================================*
*      FUNCTION:     CalculoVelocidade                                                             *
*      DESCRIPTION:                                                                                *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
void CalculoVelocidade(void)
{
	uint8_t Dezena, Centena, Unidade;
	static uint16_t Sys_DelayTach;

	/*------------------------- Cálculo da velocidade --------------------------*/
	if(Delay_mS(&Sys_DelayTach,1007)) /* tratamento a cada 1 seg */
	{
		VelocidadeTac = (ContadorPulsosTac*360)/FatorK;
		ContadorPulsosTac = 0;
		if((flash.msg[11][249] & 0x02) && Sys_MessagePointer==13)
		{
			Centena = VelocidadeTac/100;
			Dezena = (VelocidadeTac-Centena*100)/10;
			Unidade = VelocidadeTac-Centena*100 - Dezena*10;

			if (VelocidadeTac >= 100)
			{
				Dezena = 9;
				Unidade = 9;
			}

			Sys_ActualDisplayMessage = Sys_MessagePointer;
			msg.msg[0]=Dezena + '0';
			msg.msg[1]=Unidade + '0';
			msg.msg[2]='k';
			msg.msg[3]='m';
			msg.msg[4]='h';
			Sys_ConvertASCIItoMatriz(Sys_MessagePointer);
		}
	}
}

/*=================================================================================================*
*      FUNCTION:     HoraPiscando                                                                  *
*      DESCRIPTION:                                                                                *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
void HoraPiscando(void)
{
	static uint16_t Sys_DelayPiscaHora;

	if(Delay_mS(&Sys_DelayPiscaHora,500))
	{
		piscaHora = !piscaHora;
		if((flash.msg[11][249] & 0x01) && Sys_MessagePointer==12)
		{
			msg.cfg[MSG_INFO_EFEITO] = ROT_EFEITO_ESTATICA;
			msg.msg[0]=Sys_HoraA+ '0';
			msg.msg[1]=Sys_HoraB+ '0';

			if(piscaHora)
				msg.msg[2]=':';
			else
				msg.msg[2]=' ';

			msg.msg[3]=Sys_MinutoA+ '0';
			msg.msg[4]=Sys_MinutoB+ '0';
			Sys_ConvertASCIItoMatriz(Sys_MessagePointer);
		}
	}
}

/*=================================================================================================*
*      FUNCTION:     EntradaPosIgnicao                                                             *
*      DESCRIPTION:                                                                                *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
void EntradaPosIgnicao(void)
{
	//ENTRADA DO POS IGNICAO
	if(PORTJ.PIDR.BIT.B7)/* Estado da tecla diferente do da trava */
	{
		Sys_InputTurnOnProductCnt++;
		if(Sys_InputTurnOnProductCnt > 100)
		{
			Sys_InputTurnOnProduct = 1;
		}
	}
	else
	{
		Sys_InputTurnOnProductCnt = 0;
		Sys_InputTurnOnProduct = 0;
	}
	//FIM ENTRADA POS IGNICAO
}

/*=================================================================================================*
*      FUNCTION:     AlimentaWTD                                                                   *
*      DESCRIPTION:                                                                                *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
void AlimentaWTD(void)
{
	IWDT.IWDTRR = 0x00;
	IWDT.IWDTRR = 0xff;
}

/*=================================================================================================*
*      FUNCTION:     ControleTemperaturaInterna                                                    *
*      DESCRIPTION:                                                                                *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
void ControleTemperaturaInterna(void)
{
	static uint16_t Sys_DelayIntTemp;
	uint16_t buf;

	if(Delay_mS(&Sys_DelayIntTemp,3000)) /* tratamento a cada 3 seg */
	{
		buf = Sys_InternalTempSensor[Sys_InternalTemperatureBuffer]; /* Puxa dado da tabela encontrada em "r_user_intempsensor.h" */
		/* Valores abaixo de 1011 são negativos. Então, aplica-se offset */
		/* Obs.: feito para facilitar processamento */
		if(buf >= 1024)
		{
		buf -= 1024;
		}
		else
		{
		buf = 0;
		}
		/* Converte o valor de temperatura em caracteres */
		Sys_TempA_int  = buf / 100;
		Sys_TempB_int  = (buf - (Sys_TempA_int * 100))/10;
		Sys_TempC_int  = (buf - (Sys_TempA_int * 100) - (Sys_TempB_int * 10));
	}
}

/*=================================================================================================*
*      FUNCTION:     ControleTemperaturaExterna                                                    *
*      DESCRIPTION:                                                                                *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
void ControleTemperaturaExterna(void)
{
	static uint16_t Sys_DelayExtTemp;
	uint16_t buf;

	if(Delay_mS(&Sys_DelayExtTemp,3000)) /* tratamento a cada 3 seg */
	{
		/*--------------------------------------------------------------------------*/
		/*--------------------- Controle de temperatura externa --------------------*/
		buf = Sys_ExternalTempSensor[Sys_ExternalTemperatureBuffer-40]; /* Puxa dado da tabela encontrada em "r_user_extempsensor.h" */

		/* Valores abaixo de 775 são negativos. Então, aplica-se offset */
		/* Obs.: feito para facilitar processamento */
		if(buf >= 775)
		{
			buf -= 775;
		}
		else
		{
			buf = 0;
		}

		/* Converte o valor de temperatura em caracteres */
		Sys_TempA_ext  = buf / 100;
		Sys_TempB_ext  = (buf - (Sys_TempA_ext * 100))/10;
		Sys_TempC_ext  = (buf - (Sys_TempA_ext * 100) - (Sys_TempB_ext * 10));
	}
}

/*=================================================================================================*
*      FUNCTION:     ButtonHourUp                                                                  *
*      DESCRIPTION:                                                                                *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
void ButtonHourUp(void)
{
	static uint16_t Sys_ButtonHourUp = 0;
	static uint16_t Sys_ButtonHourUpCnt = 0;
	static uint16_t Sys_ButtonHourUpLock = 0;

	if(Sys_RTC >= 4000 && Sys_TypeOfMsgOnDisplay == SetRTC) /* Caso tenha reconhecido o botão mudando de estado por x ciclos... */
	{
		ZeraEfeito();
		Sys_RTC = 0;
		Sys_HoraEmMinutos = Sys_Horas*60 + Sys_Minutos;
		Sys_TypeOfMsgOnDisplay = NormalMessage;
	}
    else
    {
    	Sys_RTC++;
        if((PORT1.PIDR.BYTE & HOUR_UP) != Sys_ButtonHourUpLock) /* Estado da tecla diferente do da trava */
        {
        	Sys_ButtonHourUpCnt++; /* Incrementa contador de passagem */
          	if(Sys_ButtonHourUpCnt >= 40) /* Caso tenha reconhecido o botão mudando de estado por x ciclos... */
          	{
            	Sys_RTC = 0;
            	Sys_ButtonHourUp = Sys_ButtonHourUpLock = (PORT1.PIDR.BYTE & HOUR_UP); /* Assume novo estado */
            	if (!Sys_ButtonHourUp)
            	{
              		if (Sys_TypeOfMsgOnDisplay != SetRTC)
              		{
                 		Sys_TypeOfMsgOnDisplay = SetRTC;
                 		ZeraEfeito();

						for (uint8_t i=0;i<240;i++)
						{
							msg_edicao[i]=0;
						}

						Sys_DisplayFlags |= NeedToFlowWithNothing;
              		}

					if (Sys_Horas<12)
                		Sys_Horas++;
              		else
						Sys_Horas = 1;
				}
          	}
        }
        else
        {
          Sys_ButtonHourUpCnt = 0; /* zera contador, forçando a contagem a ser realizado nos ciclos corretos */
        }
	}
}

/*=================================================================================================*
*      FUNCTION:     ButtonHourDown                                                                *
*      DESCRIPTION:                                                                                *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
void ButtonHourDown(void)
{
	static uint8_t DebounceMin = 200;
	static uint8_t ContaDebounce = 0;
	static uint16_t Sys_ButtonHourDownCnt = 0;

	if (!PORT2.PIDR.BIT.B7)
	{
	  Sys_ButtonHourDownCnt++;
	  if (Sys_ButtonHourDownCnt >= DebounceMin)
	  {
		  Sys_RTC = 0;

		  if (Sys_Minutos<59)
			Sys_Minutos ++;
		  else
			  Sys_Minutos = 0;

		  Sys_ButtonHourDownCnt = 0;
		  if (Sys_TypeOfMsgOnDisplay != SetRTC)
		  {
			 Sys_TypeOfMsgOnDisplay = SetRTC;
			 ZeraEfeito();
		  }
		  if (ContaDebounce<5)
		  {
			ContaDebounce++;
		  }
		  else
		  {
			  DebounceMin = 50;
		  }
	  }
	}
	else
	{
	  DebounceMin = 200;
	  ContaDebounce = 0;
	}
}

/*=================================================================================================*
*      FUNCTION:     Efeito_PacMan                                                                 *
*      DESCRIPTION:                                                                                *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
void Efeito_PacMan(void)
{
	const uint8_t pac_aberto[8]              = {0x003C, 0x007E, 0x00FF, 0x00FF, 0x00E5, 0x00C3, 0x0002, 0x0000};
	const uint8_t pac_semiaberto[8]          = {0x003C, 0x007E, 0x00FF, 0x00FF, 0x00F5, 0x00E7, 0x0066, 0x0024};
	const uint8_t pac_fechado[8]             = {0x003C, 0x007E, 0x00FF, 0x00FF, 0x00FD, 0x00FF, 0x007E, 0x003C};
	const uint8_t pac_aberto_deitado[10]     = {0x003C, 0x007E, 0x00FF, 0x00FF, 0x00F5, 0x00E3, 0x00C1, 0x0080, 0x0000, 0x0000};
	const uint8_t pac_semiaberto_deitado[10] = {0x003C, 0x007E, 0x00FF, 0x00FF, 0x00F5, 0x00E7, 0x00E7, 0x00C7, 0x0046, 0x0004};
	const uint8_t pac_fechado_deitado[10]    = {0x003C, 0x007E, 0x00FF, 0x00FF, 0x00FD, 0x00FF, 0x00FF, 0x00FF, 0x007E, 0x003C};

	if (msg.cfg[MSG_INFO_HABILITADA])
	{
		if (!flag_efeito)
		{
			flag_efeito=1;
			LimpaTela();
		}

		if (msg.orientacao==0 || msg.orientacao==1)
		{
			for(uint32_t i=0;i<8;i++)
			{
			if (!z_efeito)
				Sys_LedMatrixBuffer[i] = pac_aberto[i];
			else if (z_efeito==1)
				Sys_LedMatrixBuffer[i] = pac_semiaberto[i];
			else
				Sys_LedMatrixBuffer[i] = pac_fechado[i];
			}

			for(uint32_t i=8;i<LED_MATRIZ_MAX_COLUMNS-1;i++)
			{
				Sys_LedMatrixBuffer[i] = Sys_LedMatrixBuffer[i+1];
			}
		}
		else
		{
			for(uint32_t i=0;i<10;i++)
			{
			if (!z_efeito)
				Sys_LedMatrixBuffer[i] = pac_aberto_deitado[i];
			else if (z_efeito==1)
				Sys_LedMatrixBuffer[i] = pac_semiaberto_deitado[i];
			else
				Sys_LedMatrixBuffer[i] = pac_fechado_deitado[i];
			}

			for(uint32_t i=10;i<LED_MATRIZ_MAX_COLUMNS-1;i++)
			{
				Sys_LedMatrixBuffer[i] = Sys_LedMatrixBuffer[i+1];
			}
		}

		y_efeito++;

		if (y_efeito<10)
			z_efeito=0;
		else if (y_efeito<15)
			z_efeito=1;
		else if (y_efeito<20)
			z_efeito=2;
		else if (y_efeito<25)
			z_efeito=1;
		else if (y_efeito>=26)
			y_efeito=0;

		if(Sys_DisplayFlags & NeedToFlowWithNothing)
		{
			Sys_LedMatrixBuffer[LED_MATRIZ_MAX_COLUMNS-1] = 0x00;
		}
		else
		{
			Sys_LedMatrixBuffer[LED_MATRIZ_MAX_COLUMNS-1] = MAT_MSG[Sys_MessageLoadControlPointer];
		}
	}
}

/*=================================================================================================*
*      FUNCTION:     Efeito_Matricial                                                              *
*      DESCRIPTION:                                                                                *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
void Efeito_Matricial(void)
{
	if (msg.cfg[MSG_INFO_HABILITADA])
	{
		if(Sys_DisplayFlags & NeedToStopFlow)
		{
			Sys_RotEstaticaEffect(0);
			for(uint32_t i=0;i<LED_MATRIZ_MAX_COLUMNS;i++)
			{
				Sys_MatrixTempEffect2[i]=0;
			}
		}
		else
		{
			if (!flag_efeito)
			{
				flag_efeito=1;
				LimpaTela();
			}

			if (x_efeito<8 && flag_efeito==1)
			{
				efeito_matricial=1;
				if (y_efeito<LED_MATRIZ_MAX_COLUMNS)
				{
					for(uint32_t i=0;i<LED_MATRIZ_MAX_COLUMNS;i++)
					{
						Sys_MatrixTempEffect2[i]=0;
					}

					if (!z_efeito)
					{
						Sys_MatrixTempEffect1[y_efeito] = (MAT_MSG[y_efeito])&w_efeito;
						Sys_MatrixTempEffect2[y_efeito] = 255&w_efeito;
					}
					else
					{
						Sys_MatrixTempEffect1[(LED_MATRIZ_MAX_COLUMNS-1)-y_efeito] = (MAT_MSG[(LED_MATRIZ_MAX_COLUMNS-1)-y_efeito])&w_efeito;
						Sys_MatrixTempEffect2[(LED_MATRIZ_MAX_COLUMNS-1)-y_efeito] = 255&w_efeito;
					}

					y_efeito++;

					for(uint32_t i=0;i<LED_MATRIZ_MAX_COLUMNS;i++)
					{
						Sys_LedMatrixBuffer[i]|=Sys_MatrixTempEffect1[i];
					}
				}

				else
				{
					y_efeito=0;
					x_efeito++;
					z_efeito=!z_efeito;
					w_efeito=(w_efeito<<1);
				}
			}
			else
			{
				x_efeito = 0;
				efeito_matricial=0;
				y_efeito=0;
				w_efeito = 1;
				flag_efeito=2;
				for(uint32_t i=0;i<LED_MATRIZ_MAX_COLUMNS;i++)
				{
					Sys_MatrixTempEffect2[i]=0;
				}
			}
		}
	}
}

/*=================================================================================================*
*      FUNCTION:     Efeito_Dissolver                                                              *
*      DESCRIPTION:                                                                                *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
void Efeito_Dissolver(void)
{
	if(Sys_DisplayFlags & NeedToStopFlow)
	{
		Sys_RotEstaticaEffect(0);
	}
	else
	{
		if (!flag_efeito)
		{
			flag_efeito=1;
			LimpaTela();
		}

		if (x_efeito<LED_MATRIZ_MAX_COLUMNS && flag_efeito==1)
		{
			z_efeito++;

			if (z_efeito>2)
				z_efeito=0;//apenas para controlar a velocidade do efeito no braço...(gambis...)

			if (!z_efeito)
			{
				for (uint8_t i=0;i<LED_MATRIZ_MAX_COLUMNS;i++)
				{
					y_efeito = rand()+1;
					Sys_MatrixTempEffect1[i]=MAT_MSG[i]&y_efeito;
				}
			}

			for (uint8_t i=0;i<LED_MATRIZ_MAX_COLUMNS;i++)
			{
				Sys_LedMatrixBuffer[i]|=Sys_MatrixTempEffect1[i];
			}
			x_efeito++;
		}
		else
		{
			x_efeito=0;
			flag_efeito=2;
		}
	}
}

/*=================================================================================================*
*      FUNCTION:     Efeito_SobrePor                                                               *
*      DESCRIPTION:                                                                                *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
void Efeito_SobrePor(void)
{
	if(Sys_DisplayFlags & NeedToStopFlow)
	{
		Sys_RotEstaticaEffect(0);
	}
	else
	{
		if (!flag_efeito)
		{
			flag_efeito=1;
			LimpaTela();
		}

		if (x_efeito<LED_MATRIZ_MAX_COLUMNS && flag_efeito==1)
		{
			Sys_MatrixTempEffect1[0]=MAT_MSG[x_efeito];
			Sys_MatrixTempEffect2[LED_MATRIZ_MAX_COLUMNS-1]=MAT_MSG[LED_MATRIZ_MAX_COLUMNS-x_efeito];

			for (uint8_t i=0;i<LED_MATRIZ_MAX_COLUMNS;i++)
			{
				Sys_LedMatrixBuffer[i]=Sys_MatrixTempEffect1[i]|Sys_MatrixTempEffect2[LED_MATRIZ_MAX_COLUMNS-1-i];
			}

			for (uint8_t i=0;i<LED_MATRIZ_MAX_COLUMNS;i++)
			{
				Sys_MatrixTempEffect1[LED_MATRIZ_MAX_COLUMNS-i]=Sys_MatrixTempEffect1[LED_MATRIZ_MAX_COLUMNS-1-i];
				Sys_MatrixTempEffect2[i]=Sys_MatrixTempEffect1[i+1];
			}

			x_efeito++;
		}
		else
		{
			x_efeito=0;
			flag_efeito=2;
		}
	}
}

/*=================================================================================================*
*      FUNCTION:     Efeito_Laser                                                                  *
*      DESCRIPTION:                                                                                *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
void Efeito_Laser(void)
{
	if(Sys_DisplayFlags & NeedToStopFlow)
	{
		Sys_RotEstaticaEffect(0);
	}
	else
	{
		if (!flag_efeito)
		{
			flag_efeito=1;
			LimpaTela();
		}

		if (x_efeito<LED_MATRIZ_MAX_COLUMNS)
		{
			if (y_efeito<8)
			{
				y_efeito++;
				z_efeito=(z_efeito<<1)+1;
				if ((MAT_MSG[x_efeito])&z_efeito)//nop
				{
					for (uint8_t i=x_efeito;i<LED_MATRIZ_MAX_COLUMNS;i++)//nop
					{
						Sys_LedMatrixBuffer[i] = 255&w_efeito; //A ULTIMA LINHA VEM DQUI....
					}
				}
				w_efeito<<=1;
				Sys_LedMatrixBuffer[x_efeito] = (MAT_MSG[x_efeito]);//&z_efeito;
			}
			else
			{
				y_efeito=0;
				z_efeito=0;
				w_efeito = 1;
				x_efeito++;//para ver de que coluna começa
				for (uint8_t i=x_efeito;i<LED_MATRIZ_MAX_COLUMNS;i++)//nop
				{
					Sys_LedMatrixBuffer[i]=0;
				}
			}
		}
		else
		{
			x_efeito = 0;
			z_efeito=0;
			w_efeito = 1;
			Sys_DisplayFlags = 0;
			Sys_DisplayFlags |= NeedToStopFlow;//colocar isso quando escolhe os efeitos... mudar nos outros tbm
		}
	}
}

/*=================================================================================================*
*      FUNCTION:     Efeito_Revelar                                                                *
*      DESCRIPTION:                                                                                *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
void Efeito_Revelar(void)
{
	if(Sys_DisplayFlags & NeedToStopFlow)
	{
		Sys_RotEstaticaEffect(0);
	}
	else
	{
		if (!flag_efeito)
		{
			flag_efeito=1;
			LimpaTela();
		}

		Sys_LedMatrixBuffer[x_efeito] = MAT_MSG[x_efeito];//-Sys_StopFlowInit+Sys_StopFlowOffset];
		x_efeito++;

		if (x_efeito>(LED_MATRIZ_MAX_COLUMNS-1))
		{
			x_efeito=0;
			Sys_DisplayFlags |= NeedToStopFlow;
		}
	}
}

/*=================================================================================================*
*      FUNCTION:     Efeito_RevelarInvertido                                                       *
*      DESCRIPTION:                                                                                *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
void Efeito_RevelarInvertido(void)
{
	if (msg.cfg[MSG_INFO_HABILITADA])
	{
		if(Sys_DisplayFlags & NeedToStopFlow)
		{
			Sys_RotEstaticaEffect(0);
		}
		else
		{
			if (!flag_efeito)
			{
				flag_efeito=1;
				for (uint8_t i=0;i<LED_MATRIZ_MAX_COLUMNS;i++)
				{
					Sys_LedMatrixBuffer[i]=255;
				}
			}

			Sys_LedMatrixBuffer[x_efeito] = MAT_MSG[x_efeito];
			x_efeito++;

			if (x_efeito>(LED_MATRIZ_MAX_COLUMNS-1))
			{
				x_efeito=0;
				Sys_DisplayFlags |= NeedToStopFlow;
			}
		}
	}
}

/*=================================================================================================*
*      FUNCTION:     Efeito_Quicando                                                               *
*      DESCRIPTION:                                                                                *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
void Efeito_Quicando(void)
{
	if(Sys_DisplayFlags & NeedToStopFlow)
	{
		Sys_RotEstaticaEffect(1);
	}
	else
	{
		if (!flag_efeito)
		{
			flag_efeito=1;
			LimpaTela();
		}

		if (Sys_MessageLoadControlPointer<8 || Sys_MessageLoadControlPointer >= 12)
		{
			for(uint32_t i=0;i<LED_MATRIZ_MAX_COLUMNS;i++)
			{
				Sys_LedMatrixBuffer[i] = Sys_LedMatrixBuffer[i]<<1; /* Rotaciona a matriz pra cima */

				/* Verifica se devemos ou não percorrer a tela com algo */
				if(Sys_DisplayFlags & NeedToFlowWithNothing)
				{
					Sys_LedMatrixBuffer[i] &= ~0x01; /* Carrega com zero o buffer */
				}
				else
				{
					if((MAT_MSG[i] & (1 << (7-Sys_MessageLoadControlPointer)))&&((i < Tamanho)))
					{
						Sys_LedMatrixBuffer[i] |= 0x01;
					}
					else
					{
						Sys_LedMatrixBuffer[i] &= ~0x01;
					}
				}
			}
		}
		else if (Sys_MessageLoadControlPointer>=8 && Sys_MessageLoadControlPointer<12)
		{
			for(uint32_t i=0;i<LED_MATRIZ_MAX_COLUMNS;i++)
			{
				Sys_LedMatrixBuffer[i] = Sys_LedMatrixBuffer[i]>>1; /* Rotaciona a matriz pra baixo */

				if(Sys_DisplayFlags & NeedToFlowWithNothing)
				{
					Sys_LedMatrixBuffer[i] &= ~0x80; /* Carrega com zero o buffer */
				}
				else
				{
					if((MAT_MSG[i] & (1 << (Sys_MessageLoadControlPointer)))&&((i < Tamanho)))
					{
						Sys_LedMatrixBuffer[i] |= 0x80;
					}
					else
					{
						Sys_LedMatrixBuffer[i] &= ~0x80;
					}
				}
			}
		}
	}
}

/*=================================================================================================*
*      FUNCTION:     Efeito_Estatico                                                               *
*      DESCRIPTION:                                                                                *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
void Efeito_Estatico(void)
{
	Sys_RotEstaticaEffect(0);
}

/*=================================================================================================*
*      FUNCTION:     Efeito_Scroll_Right                                                           *
*      DESCRIPTION:                                                                                *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
void Efeito_Scroll_Right(void)
{
	if(Sys_DisplayFlags & NeedToStopFlow)
	{
		Sys_RotEstaticaEffect(0);
	}
	else
	{
		if (!flag_efeito)//pra fazer os empurra soh comenta isso...
		{
			flag_efeito=1;
			LimpaTela();
		}

		for(uint32_t i=LED_MATRIZ_MAX_COLUMNS;i;i--)
		{
			Sys_LedMatrixBuffer[i] = Sys_LedMatrixBuffer[i-1];//FUNÇÃO DE DESLOCAR O VETOR UMA POSICAO
		}

		if(Sys_DisplayFlags & NeedToFlowWithNothing)
		{
			Sys_LedMatrixBuffer[0] = 0x00; /* Carrega com zero o buffer */
		}
		else
		{
			Sys_LedMatrixBuffer[0] = MAT_MSG[Tamanho-1-Sys_MessageLoadControlPointer];
		}
	}
}

/*=================================================================================================*
*      FUNCTION:     Efeito_Scroll_left                                                            *
*      DESCRIPTION:                                                                                *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
void Efeito_Scroll_left(void)
{
	if(Sys_DisplayFlags & NeedToStopFlow)
	{
		Sys_RotEstaticaEffect(0);
	}
	else
	{
		if (msg.cfg[MSG_INFO_EFEITO] != EMPURRAR_ESQUERDA)
		{
			if (!flag_efeito)
			{
				flag_efeito=1;
				LimpaTela();
			}
		}

		for(uint32_t i=0;i<(LED_MATRIZ_MAX_COLUMNS-1);i++)
		{
			Sys_LedMatrixBuffer[i] = Sys_LedMatrixBuffer[i+1];
		}

		if(Sys_DisplayFlags & NeedToFlowWithNothing)
		{
			Sys_LedMatrixBuffer[LED_MATRIZ_MAX_COLUMNS-1] = 0x00;
		}
		else
		{
			Sys_LedMatrixBuffer[LED_MATRIZ_MAX_COLUMNS-1] = MAT_MSG[Sys_MessageLoadControlPointer];
		}
	}
}

/*=================================================================================================*
*      FUNCTION:     Efeito_Scroll_Up                                                              *
*      DESCRIPTION:                                                                                *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
void Efeito_Scroll_Up(void)
{
	if(Sys_DisplayFlags & NeedToStopFlow)
	{
		Sys_RotEstaticaEffect(1);
	}
	else
	{
		if (msg.cfg[MSG_INFO_EFEITO] == ROT_EFEITO_CIMA)
		{
			if (!flag_efeito)
			{
				flag_efeito=1;
				LimpaTela();
			}
		}

		for(uint32_t i=0;i<LED_MATRIZ_MAX_COLUMNS;i++)
		{
			Sys_LedMatrixBuffer[i] = Sys_LedMatrixBuffer[i]<<1; /* Rotaciona a matriz pra cima */
			if(Sys_DisplayFlags & NeedToFlowWithNothing)
			{
				Sys_LedMatrixBuffer[i] &= ~0x01; /* Carrega com zero o buffer */
			}
			else
			{
				if((MAT_MSG[i] & (1 << (7-Sys_MessageLoadControlPointer)))&&((i < Tamanho)))
				{
					Sys_LedMatrixBuffer[i] |= 0x01;
				}
				else
				{
					Sys_LedMatrixBuffer[i] &= ~0x01;
				}
			}
		}
	}
}

/*=================================================================================================*
*      FUNCTION:     Efeito_Scroll_Down                                                            *
*      DESCRIPTION:                                                                                *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
void Efeito_Scroll_Down(void)
{
	if(Sys_DisplayFlags & NeedToStopFlow)
	{
		Sys_RotEstaticaEffect(1);
	}
	else
	{
		if (msg.cfg[MSG_INFO_EFEITO] == ROT_EFEITO_BAIXO)
		{
			if (!flag_efeito)
			{
				flag_efeito=1;
				LimpaTela();
			}
		}

		for(uint32_t i=0;i<LED_MATRIZ_MAX_COLUMNS;i++)
		{
			Sys_LedMatrixBuffer[i] = Sys_LedMatrixBuffer[i]>>1; /* Rotaciona a matriz pra baixo */
			if(Sys_DisplayFlags & NeedToFlowWithNothing)
			{
				Sys_LedMatrixBuffer[i] &= ~0x80;
			}
			else
			{
				if((MAT_MSG[i] & (1 << (Sys_MessageLoadControlPointer)))&&((i < Tamanho)))
				{
					Sys_LedMatrixBuffer[i] |= 0x80;
				}
				else
				{
					Sys_LedMatrixBuffer[i] &= ~0x80;
				}
			}
		}
	}
}

/*=================================================================================================*
*      FUNCTION:     isMsg_Enable                                                                  *
*      DESCRIPTION:  Le a configuração de MENSAGEM HABILITADA                                      *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
uint8_t isMsg_Enable(uint8_t msg_id) //Le a configuração de MENSAGEM HABILITADA
{
	return flash.msg[msg_id][241];
}

/*=================================================================================================*
*      FUNCTION:                                                                                   *
*      DESCRIPTION:                                                                                *
*      PARAMETERS:                                                                                 *
*      RETURN:                                                                                     *
*=================================================================================================*/
uint8_t Input_Read_CfgLevel(uint8_t msg_id) //Le a configuração de acionamenta da entrada, se baixo ou alto
{
	return flash.msg[msg_id][250];
}

/*=================================================================================================*
*      FUNCTION: Sys_GetMessagePointerNext                                                         *
*      DESCRIPTION: Obtem a proxima mensagem a ser mostrada, depende do que está habilitado/Ativo  *
*                   Inmplementa um sistema de fila de mensagem.                                    *
*                   Depende da mensagem atual, para saber qual é a proxima.                        *
*      PARAMETERS:  NONE                                                                           *
*      RETURN:  Proxima msg a ser mostrada                                                         *
*=================================================================================================*/
uint8_t Sys_GetMessagePointerNext(void)
{
	uint8_t NewMessage = 0xff;

	switch(Sys_MessagePointer){
	case 0:{

		//Mensagens por Entradas...
		if(Entrada[1] == 1)
			NewMessage = 1;
		else if(Entrada[2] == 1)
			NewMessage = 2;
		else if(Entrada[3] == 1)
			NewMessage = 3;

        //Mensagens Normais...
		else if(flash.msg[4][241])
			NewMessage = 4;
		else if(flash.msg[5][241])
			NewMessage = 5;
		else if(flash.msg[6][241])
			NewMessage = 6;
		else if(flash.msg[7][241])
			NewMessage = 7;
		else if(flash.msg[8][241])
			NewMessage = 8;
		else if(flash.msg[9][241])
			NewMessage = 9;
		else if(flash.msg[10][241])
			NewMessage = 10;
		else if(flash.msg[11][241])
			NewMessage = 11;

		//Mensagens extras...
		else if(flash.msg[11][249] & 0x01) //HORARIO              //Sys_MessagePointer == 12
			NewMessage = 12;
		else if(flash.msg[11][249] & 0x02) //VELOCIDADE           //Sys_MessagePointer == 13
			NewMessage = 13;
		else if(flash.msg[11][249] & 0x04) //TEMPERATURA INTERNA  //Sys_MessagePointer == 14
			NewMessage = 14;
		else if(flash.msg[11][249] & 0x08) //TEMPERATURA EXTERNA  //Sys_MessagePointer == 15
			NewMessage = 15;
		else
			Sys_MessagePointer = 0;

		break;
	}

	case 1:{

		if(Entrada[2] == 1)
			NewMessage = 2;
		else if(Entrada[3] == 1)
			NewMessage = 3;


		else if(flash.msg[4][241])
			NewMessage = 4;
		else if(flash.msg[5][241])
			NewMessage = 5;
		else if(flash.msg[6][241])
			NewMessage = 6;
		else if(flash.msg[7][241])
			NewMessage = 7;
		else if(flash.msg[8][241])
			NewMessage = 8;
		else if(flash.msg[9][241])
			NewMessage = 9;
		else if(flash.msg[10][241])
			NewMessage = 10;
		else if(flash.msg[11][241])
			NewMessage = 11;


		else if(flash.msg[11][249] & 0x01) //HORARIO              //Sys_MessagePointer == 12
			NewMessage = 12;
		else if(flash.msg[11][249] & 0x02) //VELOCIDADE           //Sys_MessagePointer == 13
			NewMessage = 13;
		else if(flash.msg[11][249] & 0x04) //TEMPERATURA INTERNA  //Sys_MessagePointer == 14
			NewMessage = 14;
		else if(flash.msg[11][249] & 0x08) //TEMPERATURA EXTERNA  //Sys_MessagePointer == 15
			NewMessage = 15;


		else if(Entrada[0] == 1)
			NewMessage = 0;
		else
			Sys_MessagePointer = 1;

		break;
	}

	case 2:{

		if(Entrada[3] == 1)
			NewMessage = 3;


		else if(flash.msg[4][241])
			NewMessage = 4;
		else if(flash.msg[5][241])
			NewMessage = 5;
		else if(flash.msg[6][241])
			NewMessage = 6;
		else if(flash.msg[7][241])
			NewMessage = 7;
		else if(flash.msg[8][241])
			NewMessage = 8;
		else if(flash.msg[9][241])
			NewMessage = 9;
		else if(flash.msg[10][241])
			NewMessage = 10;
		else if(flash.msg[11][241])
			NewMessage = 11;

		else if(flash.msg[11][249] & 0x01) //HORARIO              //Sys_MessagePointer == 12
			NewMessage = 12;
		else if(flash.msg[11][249] & 0x02) //VELOCIDADE           //Sys_MessagePointer == 13
			NewMessage = 13;
		else if(flash.msg[11][249] & 0x04) //TEMPERATURA INTERNA  //Sys_MessagePointer == 14
			NewMessage = 14;
		else if(flash.msg[11][249] & 0x08) //TEMPERATURA EXTERNA  //Sys_MessagePointer == 15
			NewMessage = 15;

		else if(Entrada[0] == 1)
			NewMessage = 0;
		else if(Entrada[1] == 1)
			NewMessage = 1;
		else
			Sys_MessagePointer = 2;

		break;
	}

	case 3:{

		if(flash.msg[4][241])
			NewMessage = 4;
		else if(flash.msg[5][241])
			NewMessage = 5;
		else if(flash.msg[6][241])
			NewMessage = 6;
		else if(flash.msg[7][241])
			NewMessage = 7;
		else if(flash.msg[8][241])
			NewMessage = 8;
		else if(flash.msg[9][241])
			NewMessage = 9;
		else if(flash.msg[10][241])
			NewMessage = 10;
		else if(flash.msg[11][241])
			NewMessage = 11;

		else if(flash.msg[11][249] & 0x01) //HORARIO              //Sys_MessagePointer == 12
			NewMessage = 12;
		else if(flash.msg[11][249] & 0x02) //VELOCIDADE           //Sys_MessagePointer == 13
			NewMessage = 13;
		else if(flash.msg[11][249] & 0x04) //TEMPERATURA INTERNA  //Sys_MessagePointer == 14
			NewMessage = 14;
		else if(flash.msg[11][249] & 0x08) //TEMPERATURA EXTERNA  //Sys_MessagePointer == 15
			NewMessage = 15;

		else if(Entrada[0] == 1)
			NewMessage = 0;
		else if(Entrada[1] == 1)
			NewMessage = 1;
		else if(Entrada[2] == 1)
			NewMessage = 2;
		else
			Sys_MessagePointer = 3;

		break;
	}

	case 4:{

		if(flash.msg[5][241])
			NewMessage = 5;
		else if(flash.msg[6][241])
			NewMessage = 6;
		else if(flash.msg[7][241])
			NewMessage = 7;
		else if(flash.msg[8][241])
			NewMessage = 8;
		else if(flash.msg[9][241])
			NewMessage = 9;
		else if(flash.msg[10][241])
			NewMessage = 10;
		else if(flash.msg[11][241])
			NewMessage = 11;

		else if(flash.msg[11][249] & 0x01) //HORARIO              //Sys_MessagePointer == 12
			NewMessage = 12;
		else if(flash.msg[11][249] & 0x02) //VELOCIDADE           //Sys_MessagePointer == 13
			NewMessage = 13;
		else if(flash.msg[11][249] & 0x04) //TEMPERATURA INTERNA  //Sys_MessagePointer == 14
			NewMessage = 14;
		else if(flash.msg[11][249] & 0x08) //TEMPERATURA EXTERNA  //Sys_MessagePointer == 15
			NewMessage = 15;

		else if(Entrada[0] == 1)
			NewMessage = 0;
		else if(Entrada[1] == 1)
			NewMessage = 1;
		else if(Entrada[2] == 1)
			NewMessage = 2;
		else if(Entrada[3] == 1)
			NewMessage = 3;
		else
			Sys_MessagePointer = 4;

		break;
	}

	case 5:{

		if(flash.msg[6][241])
			NewMessage = 6;
		else if(flash.msg[7][241])
			NewMessage = 7;
		else if(flash.msg[8][241])
			NewMessage = 8;
		else if(flash.msg[9][241])
			NewMessage = 9;
		else if(flash.msg[10][241])
			NewMessage = 10;
		else if(flash.msg[11][241])
			NewMessage = 11;

		else if(flash.msg[11][249] & 0x01) //HORARIO              //Sys_MessagePointer == 12
			NewMessage = 12;
		else if(flash.msg[11][249] & 0x02) //VELOCIDADE           //Sys_MessagePointer == 13
			NewMessage = 13;
		else if(flash.msg[11][249] & 0x04) //TEMPERATURA INTERNA  //Sys_MessagePointer == 14
			NewMessage = 14;
		else if(flash.msg[11][249] & 0x08) //TEMPERATURA EXTERNA  //Sys_MessagePointer == 15
			NewMessage = 15;

		else if(Entrada[0] == 1)
			NewMessage = 0;
		else if(Entrada[1] == 1)
			NewMessage = 1;
		else if(Entrada[2] == 1)
			NewMessage = 2;
		else if(Entrada[3] == 1)
			NewMessage = 3;


		else if(flash.msg[4][241])
			NewMessage = 4;
		else
			Sys_MessagePointer = 5;

		break;
	}

	case 6:{

		if(flash.msg[7][241])
			NewMessage = 7;
		else if(flash.msg[8][241])
			NewMessage = 8;
		else if(flash.msg[9][241])
			NewMessage = 9;
		else if(flash.msg[10][241])
			NewMessage = 10;
		else if(flash.msg[11][241])
			NewMessage = 11;

		else if(flash.msg[11][249] & 0x01) //HORARIO              //Sys_MessagePointer == 12
			NewMessage = 12;
		else if(flash.msg[11][249] & 0x02) //VELOCIDADE           //Sys_MessagePointer == 13
			NewMessage = 13;
		else if(flash.msg[11][249] & 0x04) //TEMPERATURA INTERNA  //Sys_MessagePointer == 14
			NewMessage = 14;
		else if(flash.msg[11][249] & 0x08) //TEMPERATURA EXTERNA  //Sys_MessagePointer == 15
			NewMessage = 15;

		else if(Entrada[0] == 1)
			NewMessage = 0;
		else if(Entrada[1] == 1)
			NewMessage = 1;
		else if(Entrada[2] == 1)
			NewMessage = 2;
		else if(Entrada[3] == 1)
			NewMessage = 3;


		else if(flash.msg[4][241])
			NewMessage = 4;
		else if(flash.msg[5][241])
			NewMessage = 5;
		else
			NewMessage = 6;


		break;
	}

	case 7:{

		if(flash.msg[8][241])
			NewMessage = 8;
		else if(flash.msg[9][241])
			NewMessage = 9;
		else if(flash.msg[10][241])
			NewMessage = 10;
		else if(flash.msg[11][241])
			NewMessage = 11;

		else if(flash.msg[11][249] & 0x01) //HORARIO              //Sys_MessagePointer == 12
			NewMessage = 12;
		else if(flash.msg[11][249] & 0x02) //VELOCIDADE           //Sys_MessagePointer == 13
			NewMessage = 13;
		else if(flash.msg[11][249] & 0x04) //TEMPERATURA INTERNA  //Sys_MessagePointer == 14
			NewMessage = 14;
		else if(flash.msg[11][249] & 0x08) //TEMPERATURA EXTERNA  //Sys_MessagePointer == 15
			NewMessage = 15;

		else if(Entrada[0] == 1)
			NewMessage = 0;
		else if(Entrada[1] == 1)
			NewMessage = 1;
		else if(Entrada[2] == 1)
			NewMessage = 2;
		else if(Entrada[3] == 1)
			NewMessage = 3;


		else if(flash.msg[4][241])
			NewMessage = 4;
		else if(flash.msg[5][241])
			NewMessage = 5;
		else if(flash.msg[6][241])
			NewMessage = 6;
		else
			NewMessage = 7;

		break;
	}

	case 8:{

		if(flash.msg[9][241])
			NewMessage = 9;
		else if(flash.msg[10][241])
			NewMessage = 10;
		else if(flash.msg[11][241])
			NewMessage = 11;

		else if(flash.msg[11][249] & 0x01) //HORARIO              //Sys_MessagePointer == 12
			NewMessage = 12;
		else if(flash.msg[11][249] & 0x02) //VELOCIDADE           //Sys_MessagePointer == 13
			NewMessage = 13;
		else if(flash.msg[11][249] & 0x04) //TEMPERATURA INTERNA  //Sys_MessagePointer == 14
			NewMessage = 14;
		else if(flash.msg[11][249] & 0x08) //TEMPERATURA EXTERNA  //Sys_MessagePointer == 15
			NewMessage = 15;

		else if(Entrada[0] == 1)
			NewMessage = 0;
		else if(Entrada[1] == 1)
			NewMessage = 1;
		else if(Entrada[2] == 1)
			NewMessage = 2;
		else if(Entrada[3] == 1)
			NewMessage = 3;


		else if(flash.msg[4][241])
			NewMessage = 4;
		else if(flash.msg[5][241])
			NewMessage = 5;
		else if(flash.msg[6][241])
			NewMessage = 6;
		else if(flash.msg[7][241])
			NewMessage = 7;
		else
			NewMessage = 8;

		break;
	}

	case 9:{

		if(flash.msg[10][241])
			NewMessage = 10;
		else if(flash.msg[11][241])
			NewMessage = 11;

		else if(flash.msg[11][249] & 0x01) //HORARIO              //Sys_MessagePointer == 12
			NewMessage = 12;
		else if(flash.msg[11][249] & 0x02) //VELOCIDADE           //Sys_MessagePointer == 13
			NewMessage = 13;
		else if(flash.msg[11][249] & 0x04) //TEMPERATURA INTERNA  //Sys_MessagePointer == 14
			NewMessage = 14;
		else if(flash.msg[11][249] & 0x08) //TEMPERATURA EXTERNA  //Sys_MessagePointer == 15
			NewMessage = 15;

		else if(Entrada[0] == 1)
			NewMessage = 0;
		else if(Entrada[1] == 1)
			NewMessage = 1;
		else if(Entrada[2] == 1)
			NewMessage = 2;
		else if(Entrada[3] == 1)
			NewMessage = 3;


		else if(flash.msg[4][241])
			NewMessage = 4;
		else if(flash.msg[5][241])
			NewMessage = 5;
		else if(flash.msg[6][241])
			NewMessage = 6;
		else if(flash.msg[7][241])
			NewMessage = 7;
		else if(flash.msg[8][241])
			NewMessage = 8;
		else
			NewMessage = 9;

		break;
	}

	case 10:{

		if(flash.msg[11][241])
			NewMessage = 11;

		else if(flash.msg[11][249] & 0x01) //HORARIO              //Sys_MessagePointer == 12
			NewMessage = 12;
		else if(flash.msg[11][249] & 0x02) //VELOCIDADE           //Sys_MessagePointer == 13
			NewMessage = 13;
		else if(flash.msg[11][249] & 0x04) //TEMPERATURA INTERNA  //Sys_MessagePointer == 14
			NewMessage = 14;
		else if(flash.msg[11][249] & 0x08) //TEMPERATURA EXTERNA  //Sys_MessagePointer == 15
			NewMessage = 15;

		else if(Entrada[0] == 1)
			NewMessage = 0;
		else if(Entrada[1] == 1)
			NewMessage = 1;
		else if(Entrada[2] == 1)
			NewMessage = 2;
		else if(Entrada[3] == 1)
			NewMessage = 3;

		else if(flash.msg[4][241])
			NewMessage = 4;
		else if(flash.msg[5][241])
			NewMessage = 5;
		else if(flash.msg[6][241])
			NewMessage = 6;
		else if(flash.msg[7][241])
			NewMessage = 7;
		else if(flash.msg[8][241])
			NewMessage = 8;
		else if(flash.msg[9][241])
			NewMessage = 9;
		else
			NewMessage = 10;

		break;
	}

	case 11:{

		if(flash.msg[11][249] & 0x01)      //HORARIO              //Sys_MessagePointer == 12
			NewMessage = 12;
		else if(flash.msg[11][249] & 0x02) //VELOCIDADE           //Sys_MessagePointer == 13
			NewMessage = 13;
		else if(flash.msg[11][249] & 0x04) //TEMPERATURA INTERNA  //Sys_MessagePointer == 14
			NewMessage = 14;
		else if(flash.msg[11][249] & 0x08) //TEMPERATURA EXTERNA  //Sys_MessagePointer == 15
			NewMessage = 15;

		else if(Entrada[0] == 1)
			NewMessage = 0;
		else if(Entrada[1] == 1)
			NewMessage = 1;
		else if(Entrada[2] == 1)
			NewMessage = 2;
		else if(Entrada[3] == 1)
			NewMessage = 3;

		else if(flash.msg[4][241])
			NewMessage = 4;
		else if(flash.msg[5][241])
			NewMessage = 5;
		else if(flash.msg[6][241])
			NewMessage = 6;
		else if(flash.msg[7][241])
			NewMessage = 7;
		else if(flash.msg[8][241])
			NewMessage = 8;
		else if(flash.msg[9][241])
			NewMessage = 9;
		else if(flash.msg[10][241])
			NewMessage = 10;
		else
			NewMessage = 11;

		break;
	}

	case 12:{ //HORARIO

		if(flash.msg[11][249] & 0x02)      //VELOCIDADE           //Sys_MessagePointer == 13
			NewMessage = 13;
		else if(flash.msg[11][249] & 0x04) //TEMPERATURA INTERNA  //Sys_MessagePointer == 14
			NewMessage = 14;
		else if(flash.msg[11][249] & 0x08) //TEMPERATURA EXTERNA  //Sys_MessagePointer == 15
			NewMessage = 15;

		else if(Entrada[0] == 1)
			NewMessage = 0;
		else if(Entrada[1] == 1)
			NewMessage = 1;
		else if(Entrada[2] == 1)
			NewMessage = 2;
		else if(Entrada[3] == 1)
			NewMessage = 3;

        //Mensagens Normais...
		else if(flash.msg[4][241])
			NewMessage = 4;
		else if(flash.msg[5][241])
			NewMessage = 5;
		else if(flash.msg[6][241])
			NewMessage = 6;
		else if(flash.msg[7][241])
			NewMessage = 7;
		else if(flash.msg[8][241])
			NewMessage = 8;
		else if(flash.msg[9][241])
			NewMessage = 9;
		else if(flash.msg[10][241])
			NewMessage = 10;
		else if(flash.msg[11][241])
			NewMessage = 11;
		else
			NewMessage = 12;

		break;
	}

	case 13:{ //VELOCIDADE

		if(flash.msg[11][249] & 0x04) //TEMPERATURA INTERNA  //Sys_MessagePointer == 14
			NewMessage = 14;
		else if(flash.msg[11][249] & 0x08) //TEMPERATURA EXTERNA  //Sys_MessagePointer == 15
			NewMessage = 15;

		else if(Entrada[0] == 1)
			NewMessage = 0;
		else if(Entrada[1] == 1)
			NewMessage = 1;
		else if(Entrada[2] == 1)
			NewMessage = 2;
		else if(Entrada[3] == 1)
			NewMessage = 3;

        //Mensagens Normais...
		else if(flash.msg[4][241])
			NewMessage = 4;
		else if(flash.msg[5][241])
			NewMessage = 5;
		else if(flash.msg[6][241])
			NewMessage = 6;
		else if(flash.msg[7][241])
			NewMessage = 7;
		else if(flash.msg[8][241])
			NewMessage = 8;
		else if(flash.msg[9][241])
			NewMessage = 9;
		else if(flash.msg[10][241])
			NewMessage = 10;
		else if(flash.msg[11][241])
			NewMessage = 11;
		else
			NewMessage = 13;


		break;
	}

	case 14:{ //TEMPERATURA INTERNA

		if(flash.msg[11][249] & 0x08) //TEMPERATURA EXTERNA  //Sys_MessagePointer == 15
			NewMessage = 15;

		else if(Entrada[0] == 1)
			NewMessage = 0;
		else if(Entrada[1] == 1)
			NewMessage = 1;
		else if(Entrada[2] == 1)
			NewMessage = 2;
		else if(Entrada[3] == 1)
			NewMessage = 3;

        //Mensagens Normais...
		else if(flash.msg[4][241])
			NewMessage = 4;
		else if(flash.msg[5][241])
			NewMessage = 5;
		else if(flash.msg[6][241])
			NewMessage = 6;
		else if(flash.msg[7][241])
			NewMessage = 7;
		else if(flash.msg[8][241])
			NewMessage = 8;
		else if(flash.msg[9][241])
			NewMessage = 9;
		else if(flash.msg[10][241])
			NewMessage = 10;
		else if(flash.msg[11][241])
			NewMessage = 11;

		else if(flash.msg[11][249] & 0x01) //HORARIO              //Sys_MessagePointer == 12
			NewMessage = 12;
		else if(flash.msg[11][249] & 0x02) //VELOCIDADE           //Sys_MessagePointer == 13
			NewMessage = 13;
		else
			NewMessage = 14;

		break;
	}

	case 15:{ //TEMPERATURA EXTERNA

		if(Entrada[0] == 1)
			NewMessage = 0;
		else if(Entrada[1] == 1)
			NewMessage = 1;
		else if(Entrada[2] == 1)
			NewMessage = 2;
		else if(Entrada[3] == 1)
			NewMessage = 3;

        //Mensagens Normais...
		else if(flash.msg[4][241])
			NewMessage = 4;
		else if(flash.msg[5][241])
			NewMessage = 5;
		else if(flash.msg[6][241])
			NewMessage = 6;
		else if(flash.msg[7][241])
			NewMessage = 7;
		else if(flash.msg[8][241])
			NewMessage = 8;
		else if(flash.msg[9][241])
			NewMessage = 9;
		else if(flash.msg[10][241])
			NewMessage = 10;
		else if(flash.msg[11][241])
			NewMessage = 11;

		else if(flash.msg[11][249] & 0x01) //HORARIO              //Sys_MessagePointer == 12
			NewMessage = 12;
		else if(flash.msg[11][249] & 0x02) //VELOCIDADE           //Sys_MessagePointer == 13
			NewMessage = 13;
		else if(flash.msg[11][249] & 0x04) //TEMPERATURA INTERNA  //Sys_MessagePointer == 14
			NewMessage = 14;
		else
			NewMessage = 15;

		break;
	}

	default:{
		NewMessage = 0;
		break;
	}
	}

	return NewMessage;
}
/*============================================================================*/
/*      END OF r_user_hwctrl.c                                                */
/*============================================================================*/
