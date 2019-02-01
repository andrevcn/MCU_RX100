#ifndef __MAIN_H
#define __MAIN_H

/* Inclusoes de arquivos de cabecalho ------------------------------------------------------------*/
#include "r_cg_macrodriver.h"
#include "r_cg_s12adb.h"


#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

#define _c 		261
#define _d 		294
#define _e 		329
#define _f 		349
#define _g 		391
#define _gS 	415
#define _a 		440
#define _aS 	455
#define _b 		466
#define _cH 	523
#define _cSH 	554
#define _dH 	587
#define _dSH 	622
#define _eH 	659
#define _fH 	698
#define _fSH 	740
#define _gH 	784
#define _gSH 	830
#define _aH 	880


/*
Pinagem utilizada no prototipo da jiga de testes para leitura de corrente das luminarias:

****DISPLAY 16x2***********************
	RS	Port4 bit0
	EN	PortE bit1
	D7	PortB bit0
	D6	PortA bit6
	D5	PortA bit3
	D4	PortA bit1

****Buzzer*****************************
	Portc bit7

****PWM********************************
	P17

****Analog In (Leitura de corrente)****
	AN11 (PE3)

****Teclas*****************************
	Tecla 1	"Enter"	Port4 bit1
	Tecla 2 "Mais"	Port4 bit2
	Tecla 3 "Menos" Port4 bit6

****MOSFET****************************
	MOSFET DE PULSO 	PortE bi7

****I2C****************************
	SCL0 -> P16
	SDA0 -> P17  ---- Conflito com PWM TODO: Ver depois !!!!
*/

/* Prototipo de funções-------------- ------------------------------------------------------------*/
void FDL_WriteControl (void);
void FDL_LoadControl (void);
void OpenMsg(uint8_t ptrMsg);
void SaveMsg(uint8_t ptrMsg);
void FDL_Open (void);
void FDL_Close (void);
void Fcn_LoadDefault(void);
void R_MAIN_UserInit(void);
uint32_t millis(void);
void __delay_ms(uint32_t compare);
void TeclaEnter_MainTask(void);
void TeclaEnter_TestTask(void);
void TeclaMais_MainTask(void);
void TeclaMenos_MainTask(void);
void TeclaEsc_MainTask(void);
void Menu_MainTask(void);
void Bitmap_Sample1(void);
void Bitmap_Sample2(void);
void Bitmap_Sample3(void);
void Test_PWM(void);
void Set_DutyCycle(uint16_t duty);
void delayMicroseconds(uint16_t tempo);
void buzz(long frequency, long length);
void Sing_MarioTheme(void);
void Sing_MarchaImperial(void);
void DBG_MainTask(void);
void P17_Cfg_Output(void);
void P17_Cfg_Input(void);
void P16_Cfg_Output(void);
void P16_Cfg_Input(void);
void SDA_Cfg_Output(void);
void SDA_Cfg_Input(void);
void PWM_MainTask(void);
int checkButton_Enter(void);

#endif /*__ENTRADAS_H */


/*****END OF FILE****/

