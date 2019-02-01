/*Includes----------------------------------------------------------------------------------------*/
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"
#include "r_cg_cmt.h"
#include "r_cg_iwdt.h"
#include "r_cg_s12adb.h"
#include "r_cg_mtu2.h"
#include "r_user_hwrctrl.h"
#include "r_usb_ctypedef.h"             /* Type define */
#include "r_usb_ckernelid.h"            /* Kernel ID definition */
#include "r_usb_usrconfig.h"            /* System definition, USB-FW Library Header */
#include "r_usb_api.h"                  /* USB API public function header */
#include "hw_resource.h"
#include "fdl_descriptor.h"
#include "r_fdl_types.h"
#include "r_fdl.h"
#include "r_fdl_env.h"
#include "r_fdl_global.h"
#include "r_cg_lpc.h"
#include "r_cg_userdefine.h"
#include "r_cg_main.h"
#include "HD44780.h"
#include "LCDBitmap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Buzzer.h"
#include "Entradas.h"
#include "SoftwareI2C.h"
#include "INA219.h"
#include "r_gpio_rx111.h"


/*Global variables -------------------------------------------------------------------------------*/
uint8_t msg_edicao[240];
Flash flash;
USB_STATIC volatile uint8_t g_usb_suspend_flag;
USB_STATIC volatile uint8_t g_usb_int_key_flag;
extern uint8_t Sys_PWMlatchEn;
extern unsigned int FatorK;
extern uint16_t Sys_TickTimer;
extern void     usb_hsmpl_main_init(void);
extern void     usb_hhid_MainTask(void);
extern void     R_usb_hhid_task(void);
extern void     usb_hmsc_Task(void);
extern void     usb_hmsc_StrgDriveTask(void);
extern void     usb_hmsc_SampleAplTask(void);
extern void     usb_hmsc_task_start( void );

#define PIN_MAX_CNT		25

#ifndef TECLA_ENTER
#define TECLA_ENTER		PORT4.PIDR.BIT.B1
#endif

#ifndef TECLA_MAIS
#define TECLA_MAIS		PORT4.PIDR.BIT.B2
#endif

#ifndef TECLA_MENOS
#define TECLA_MENOS		PORT4.PIDR.BIT.B6
#endif

#ifndef TECLA_ESC
#define TECLA_ESC		PORTE.PIDR.BIT.B0
#endif

#ifndef LED_PIN
#define LED_PIN
#endif

#ifndef BUZ_PIN
#define BUZ_PIN
#endif

// Sets how long to run each sample
#define sample 4000

//Mario main them tempo
int tempo[] = {
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
};

//Mario main theme melody
int melody[] = {
	NOTE_E7, NOTE_E7, 0, NOTE_E7,
	0, NOTE_C7, NOTE_E7, 0,
	NOTE_G7, 0, 0,  0,
	NOTE_G6, 0, 0, 0,

	NOTE_C7, 0, 0, NOTE_G6,
	0, 0, NOTE_E6, 0,
	0, NOTE_A6, 0, NOTE_B6,
	0, NOTE_AS6, NOTE_A6, 0,

	NOTE_G6, NOTE_E7, NOTE_G7,
	NOTE_A7, 0, NOTE_F7, NOTE_G7,
	0, NOTE_E7, 0, NOTE_C7,
	NOTE_D7, NOTE_B6, 0, 0,

	NOTE_C7, 0, 0, NOTE_G6,
	0, 0, NOTE_E6, 0,
	0, NOTE_A6, 0, NOTE_B6,
	0, NOTE_AS6, NOTE_A6, 0,

	NOTE_G6, NOTE_E7, NOTE_G7,
	NOTE_A7, 0, NOTE_F7, NOTE_G7,
	0, NOTE_E7, 0, NOTE_C7,
	NOTE_D7, NOTE_B6, 0, 0
};

volatile uint16_t  calc_duty_percent = 0U;
volatile uint16_t tgrb_current_value = 1U;
volatile uint8_t  FSM_App = 0U;

char str_Disp[16];
char *str_menu[] = {(char*)"Luminaria 0     ", //Posição Zero existe mas não é usada!
					(char*)"Luminaria 1     ",
					(char*)"Luminaria 2     ",
					(char*)"Luminaria 3     ",
					(char*)"Luminaria 4     ",
					(char*)"Luminaria 5     ",
					(char*)"Luminaria 6     ",
					(char*)"Luminaria 7     ",
					(char*)"Luminaria 8     "};

uint8_t FSM_Menu = 0;
uint8_t menu_index = 1;
uint16_t DelayApp;
uint32_t debounceDelay = 50;

uint8_t  PinEnterState = 1;
uint8_t  PinEnterStateOld = 1;
uint8_t  PinEnterCnt = 0;
uint8_t  TeclaEnter = 0;
uint32_t PinEnterlastDebounceTime = 0;

uint8_t  PinMaisState = 1;
uint8_t  PinMaisStateOld = 1;
uint8_t  PinMaisCnt = 0;
uint8_t  TeclaMais = 0;
uint32_t PinMaislastDebounceTime = 0;

uint8_t  PinMenosState = 1;
uint8_t  PinMenosStateOld = 1;
uint8_t  PinMenosCnt = 0;
uint8_t  TeclaMenos = 0;
uint32_t PinMenoslastDebounceTime = 0;

uint8_t PinEscState = 1;
uint8_t PinEscStateOld = 1;
uint8_t PinEscCnt = 0;
uint8_t TeclaEsc = 0;

uint8_t I2C_Value = 0;
uint8_t I2C_Value1 = 0;
uint8_t I2C_Value2 = 0;
uint8_t I2C_Value3 = 0;
uint8_t I2C_Value4 = 0;

uint32_t StepMotorDelay = 5;


#define CLICK_EVENT 		1
#define DOUBLE_CLICK_EVENT	2
#define HOLD_EVENT 			3
#define LONG_HOLD_EVENT		4


#define TRANSISTOR_Q2 		GPIO_PORT_B_PIN_1
#define TRANSISTOR_Q1		GPIO_PORT_E_PIN_4
#define TRANSISTOR_Q4		GPIO_PORT_E_PIN_7
#define TRANSISTOR_Q3		GPIO_PORT_B_PIN_5
#define TRANSISTOR_Q6		GPIO_PORT_C_PIN_4
#define TRANSISTOR_Q5		GPIO_PORT_C_PIN_5
#define TRANSISTOR_Q8		GPIO_PORT_C_PIN_6
#define TRANSISTOR_Q7		GPIO_PORT_C_PIN_7


// Button timing variables
int debounce = 20;          	// ms debounce period to prevent flickering when pressing or releasing the button
int DCgap = 250;            	// max ms between clicks for a double click event
int holdTime = 1000;        	// ms hold period: how long to wait for press+hold event
int longHoldTime = 3000;    	// ms long hold period: how long to wait for press+hold event

// Button variables
uint8_t buttonVal = HIGH;   	// value read from button
uint8_t buttonLast = HIGH;  	// buffered value of the button's previous state
uint8_t DCwaiting = 0;  		// whether we're waiting for a double click (down)
uint8_t DConUp = 0;     		// whether to register a double click on next release, or whether to wait and click
uint8_t singleOK = 1;    		// whether it's OK to do a single click
long downTime = -1;         	// time the button was pressed down
long upTime = -1;           	// time the button was released
uint8_t ignoreUp = 0;   		// whether to ignore the button release because the click+hold was triggered
uint8_t waitForUp = 0;        	// when held, whether to wait for the up event
uint8_t holdEventPast = 0;    	// whether or not the hold event happened already
uint8_t longHoldEventPast = 0;	// whether or not the long hold event happened already

extern uint8_t ShuntCurrent;

/* Set option bytes */
#pragma location = "OPTBYTE"
__root const uint32_t OFS0_location = 0xFFFFEF29UL;
#pragma location = "OPTBYTE"
__root const uint32_t OFS1_location = 0xFFFFFF7FUL;


unsigned char teste_cc=0;
unsigned char ERRO_USB=0;
extern uint16_t Sys_TypeOfMsgOnDisplay;
extern void R_ICU_IRQ5_Start(void);
void StepMotor_TestTask(void);
void StepMotor_ResetPins(void);


void main(void)
{
	R_MAIN_UserInit();
  	R_CMT0_Start(); 	/* Inicia timer da função Delay_mS */
	//BUZ_PinInit();
	//HD44780_Init(16,2);
	//LCDBitmap_Begin(0,0);
	//HD44780_Puts(0,0, (char*)str_menu[menu_index]);
	//SoftwareI2C_Init();
	//INA219_Init(INA219_ADDRESS);

	//sprintf(str_Disp, "DUTY: %d  ", calc_duty_percent);
	//HD44780_Puts(0,0, (char*)str_Disp);


	R_GPIO_PinDirectionSet(GPIO_PORT_B_PIN_1, GPIO_DIRECTION_OUTPUT);
	R_GPIO_PinDirectionSet(GPIO_PORT_E_PIN_4, GPIO_DIRECTION_OUTPUT);
	R_GPIO_PinDirectionSet(GPIO_PORT_E_PIN_7, GPIO_DIRECTION_OUTPUT);
	R_GPIO_PinDirectionSet(GPIO_PORT_B_PIN_5, GPIO_DIRECTION_OUTPUT);
	R_GPIO_PinDirectionSet(GPIO_PORT_C_PIN_4, GPIO_DIRECTION_OUTPUT);
	R_GPIO_PinDirectionSet(GPIO_PORT_C_PIN_5, GPIO_DIRECTION_OUTPUT);
	R_GPIO_PinDirectionSet(GPIO_PORT_C_PIN_6, GPIO_DIRECTION_OUTPUT);
	R_GPIO_PinDirectionSet(GPIO_PORT_C_PIN_7, GPIO_DIRECTION_OUTPUT);

	StepMotor_ResetPins();

  	while(1U)
  	{
		//TeclaEnter_MainTask();
		//TeclaMais_MainTask();
		//TeclaMenos_MainTask();
		//Menu_MainTask();
		//ADC_MainTask();
		StepMotor_TestTask();
		//PWM_MainTask();
		//TeclaEnter_TestTask();
  	}
}

void StepMotor_TestTask(void)
{
	switch(FSM_App){
	case 0:
		FSM_App = 1;
		break;

	case 1:
		R_GPIO_PinWrite(TRANSISTOR_Q1, GPIO_LEVEL_HIGH);
		R_GPIO_PinWrite(TRANSISTOR_Q2, GPIO_LEVEL_LOW);
		R_GPIO_PinWrite(TRANSISTOR_Q3, GPIO_LEVEL_LOW);
		R_GPIO_PinWrite(TRANSISTOR_Q4, GPIO_LEVEL_HIGH);
		R_GPIO_PinWrite(TRANSISTOR_Q5, GPIO_LEVEL_HIGH);
		R_GPIO_PinWrite(TRANSISTOR_Q6, GPIO_LEVEL_LOW);
		R_GPIO_PinWrite(TRANSISTOR_Q7, GPIO_LEVEL_LOW);
		R_GPIO_PinWrite(TRANSISTOR_Q8, GPIO_LEVEL_HIGH);
		__delay_ms(StepMotorDelay);
		FSM_App = 2;
		break;

	case 2:
		R_GPIO_PinWrite(TRANSISTOR_Q1, GPIO_LEVEL_LOW);
		R_GPIO_PinWrite(TRANSISTOR_Q2, GPIO_LEVEL_HIGH);
		R_GPIO_PinWrite(TRANSISTOR_Q3, GPIO_LEVEL_HIGH);
		R_GPIO_PinWrite(TRANSISTOR_Q4, GPIO_LEVEL_LOW);
		R_GPIO_PinWrite(TRANSISTOR_Q5, GPIO_LEVEL_HIGH);
		R_GPIO_PinWrite(TRANSISTOR_Q6, GPIO_LEVEL_LOW);
		R_GPIO_PinWrite(TRANSISTOR_Q7, GPIO_LEVEL_LOW);
		R_GPIO_PinWrite(TRANSISTOR_Q8, GPIO_LEVEL_HIGH);
		__delay_ms(StepMotorDelay);
		FSM_App = 3;
		break;

	case 3:
		R_GPIO_PinWrite(TRANSISTOR_Q1, GPIO_LEVEL_LOW);
		R_GPIO_PinWrite(TRANSISTOR_Q2, GPIO_LEVEL_HIGH);
		R_GPIO_PinWrite(TRANSISTOR_Q3, GPIO_LEVEL_HIGH);
		R_GPIO_PinWrite(TRANSISTOR_Q4, GPIO_LEVEL_LOW);
		R_GPIO_PinWrite(TRANSISTOR_Q5, GPIO_LEVEL_LOW);
		R_GPIO_PinWrite(TRANSISTOR_Q6, GPIO_LEVEL_HIGH);
		R_GPIO_PinWrite(TRANSISTOR_Q7, GPIO_LEVEL_HIGH);
		R_GPIO_PinWrite(TRANSISTOR_Q8, GPIO_LEVEL_LOW);
		__delay_ms(StepMotorDelay);
		FSM_App = 4;
		break;

	case 4:
		R_GPIO_PinWrite(TRANSISTOR_Q1, GPIO_LEVEL_HIGH);
		R_GPIO_PinWrite(TRANSISTOR_Q2, GPIO_LEVEL_LOW);
		R_GPIO_PinWrite(TRANSISTOR_Q3, GPIO_LEVEL_LOW);
		R_GPIO_PinWrite(TRANSISTOR_Q4, GPIO_LEVEL_HIGH);
		R_GPIO_PinWrite(TRANSISTOR_Q5, GPIO_LEVEL_LOW);
		R_GPIO_PinWrite(TRANSISTOR_Q6, GPIO_LEVEL_HIGH);
		R_GPIO_PinWrite(TRANSISTOR_Q7, GPIO_LEVEL_HIGH);
		R_GPIO_PinWrite(TRANSISTOR_Q8, GPIO_LEVEL_LOW);
		__delay_ms(StepMotorDelay);
		FSM_App = 1;
		break;

	case 20:
		StepMotor_ResetPins();
		break;
	}
}

void StepMotor_ResetPins(void)
{
	R_GPIO_PinWrite(TRANSISTOR_Q1, GPIO_LEVEL_LOW);
	R_GPIO_PinWrite(TRANSISTOR_Q2, GPIO_LEVEL_LOW);
	R_GPIO_PinWrite(TRANSISTOR_Q3, GPIO_LEVEL_LOW);
	R_GPIO_PinWrite(TRANSISTOR_Q4, GPIO_LEVEL_LOW);
	R_GPIO_PinWrite(TRANSISTOR_Q5, GPIO_LEVEL_LOW);
	R_GPIO_PinWrite(TRANSISTOR_Q6, GPIO_LEVEL_LOW);
	R_GPIO_PinWrite(TRANSISTOR_Q7, GPIO_LEVEL_LOW);
	R_GPIO_PinWrite(TRANSISTOR_Q8, GPIO_LEVEL_LOW);
}

int checkButton_Enter(void)
{
   int event = 0;

   buttonVal = TECLA_ENTER;

   // Button pressed down
   if (buttonVal == LOW && buttonLast == HIGH && (millis() - upTime) > debounce)
   {
       downTime = millis();
       ignoreUp = 0;
       waitForUp = 0;
       singleOK = 1;
       holdEventPast = 0;
       longHoldEventPast = 0;

       if ((millis()-upTime) < DCgap && DConUp == 0 && DCwaiting == 1)
		   DConUp = 1;
       else
		   DConUp = 0;

	   DCwaiting = 0;
   }
   // Button released
   else if (buttonVal == HIGH && buttonLast == LOW && (millis() - downTime) > debounce)
   {
       if (!ignoreUp)
       {
           upTime = millis();

           if (DConUp == 0)
			   DCwaiting = 1;
           else
           {
               event = 2;
               DConUp = 0;
               DCwaiting = 0;
               singleOK = 0;
           }
       }
   }

   // Test for normal click event: DCgap expired
   if ( buttonVal == HIGH && (millis()-upTime) >= DCgap && DCwaiting == 1 && DConUp == 0 && singleOK == 1 && event != 2)
   {
       event = 1;
       DCwaiting = 0;
   }

   // Test for hold
   if (buttonVal == LOW && (millis() - downTime) >= holdTime)
   {
       // Trigger "normal" hold
       if (!holdEventPast)
       {
           event = 3;
           waitForUp = 1;
           ignoreUp = 1;
           DConUp = 0;
           DCwaiting = 0;
           holdEventPast = 1;
       }

       // Trigger "long" hold
       if ((millis() - downTime) >= longHoldTime)
       {
           if (!longHoldEventPast)
           {
               event = 4;
               longHoldEventPast = 1;
           }
       }
   }
   buttonLast = buttonVal;
   return event;
}

void PWM_MainTask(void)
{
	if(TeclaMais == 1)
	{
		TeclaMais = 0;

		if(calc_duty_percent < 100)
			calc_duty_percent++;

		Set_DutyCycle(calc_duty_percent);
		sprintf(str_Disp, "DUTY: %d  ", calc_duty_percent);
		HD44780_Puts(0,0, (char*)str_Disp);
	}

	if(TeclaMenos == 1)
	{
		TeclaMenos = 0;

		if(calc_duty_percent > 0)
			calc_duty_percent--;

		Set_DutyCycle(calc_duty_percent);
		sprintf(str_Disp, "DUTY: %d  ", calc_duty_percent);
		HD44780_Puts(0,0, (char*)str_Disp);
	}
}

void P17_Cfg_Output(void) //SDA
{
	PORT1.PDR.BYTE |= _80_Pm7_MODE_OUTPUT;
}

void P17_Cfg_Input(void) //SDA
{
	PORT1.PDR.BYTE |= _00_Pm7_MODE_INPUT;
}

void SDA_Cfg_Output(void) //SDA
{
	PORT1.PDR.BYTE |= _80_Pm7_MODE_OUTPUT;
}

void SDA_Cfg_Input(void) //SDA
{
	PORT1.PDR.BYTE |= _00_Pm7_MODE_INPUT;
}

void P16_Cfg_Output(void) //SCL
{
	PORT1.PDR.BYTE |= _40_Pm6_MODE_OUTPUT;
}

void P16_Cfg_Input(void) //SCL
{
	PORT1.PDR.BYTE = _00_Pm6_MODE_INPUT;
}

void DBG_MainTask(void)
{
	switch(FSM_App){
	case 0:
		break;

	case 1:
		R_MTU2_U0_C3_Start();
		FSM_App = 0;
		break;

	case 2:
		R_MTU2_U0_C3_Stop();
		FSM_App = 0;
		break;

	case 3:
		Set_DutyCycle(calc_duty_percent);
		FSM_App = 0;
		break;

	case 4:
		tgrb_current_value = MTU3.TGRB;
		FSM_App = 0;
		break;
	case 5:
		Sing_MarioTheme();
		FSM_App = 0;
		break;

	case 6:
		Sing_MarchaImperial();
		FSM_App = 0;
		break;

	case 7:
		SoftwareI2C_sendByte(I2C_Value);
		FSM_App = 0;
		break;
	case 8:
		PORT1.PODR.BIT.B7 = 1;
		FSM_App = 0;
		break;

	case 9:
		PORT1.PODR.BIT.B7 = 0;
		FSM_App = 0;
		break;

	case 10:
		SoftwareI2C_sendByte(I2C_Value1);
		__delay_ms(5);
		break;

	case 11:
		INA219_WriteRegister(INA219_REG_CALIBRATION, 4096);
		break;

	case 12:
		SoftwareI2C_sendStart();
		SoftwareI2C_sendByte(I2C_Value1);
		SoftwareI2C_sendStop();
		__delay_ms(5);
		break;

	case 13: //Inicializa pino do buzzer usando library da renesas.
		R_GPIO_PinDirectionSet(GPIO_PORT_C_PIN_7, GPIO_DIRECTION_OUTPUT);
		FSM_App = 0;
		break;

	case 14: //Liga pino do buzzer usando library da renesas.
		R_GPIO_PinWrite(GPIO_PORT_C_PIN_7, GPIO_LEVEL_HIGH);
		FSM_App = 0;
		break;

	case 15: //Desliga pino do buzzer usando library da renesas.
		R_GPIO_PinWrite(GPIO_PORT_C_PIN_7, GPIO_LEVEL_LOW);
		FSM_App = 0;
		break;

	case 20:



		break;

	default:
		FSM_App = 0;
		break;
	}
}

void Set_DutyCycle(uint16_t duty)
{
	float fRasc;

	/* Stop the TCNT counter */
    MTU.TSTR.BIT.CST3 = 0x0;

	if(duty > 100)
		duty = 100;

	fRasc = (float) MTU3.TGRA / 100;
    fRasc = duty * fRasc;

	MTU3.TGRB = (uint16_t)fRasc;

	/* Start the TCNT counter */
    MTU.TSTR.BIT.CST3 = 0x1;
}

void Menu_MainTask(void)
{
	switch(FSM_Menu)
	{
		case 0:
		{
			if(TeclaEnter == 1)
			{
				TeclaEnter = 0;
				HD44780_Puts(0,1, (char*)"                ");
				HD44780_Puts(0,1, (char*)"TESTE.          ");
				while(Delay_mS(&DelayApp, 800) == 0);
				HD44780_Puts(0,1, (char*)"TESTE..         ");
				while(Delay_mS(&DelayApp, 800) == 0);
				HD44780_Puts(0,1, (char*)"TESTE...        ");
				while(Delay_mS(&DelayApp, 800) == 0);

				if(ShuntCurrent == 0)
				{
					HD44780_Puts(0,1, (char*)"TESTE...ERRO    ");
					buzz(500, 400);
				}

				if(ShuntCurrent > 0)
				{
					switch(menu_index){
					case 1:
						sprintf(str_Disp, "TESTE..OK [%dmA]", ShuntCurrent);
						HD44780_Puts(0,1, (char*)str_Disp);
						break;
					case 2:
						HD44780_Puts(0,1, (char*)"20mA            ");
						break;
					case 3:
						HD44780_Puts(0,1, (char*)"30mA            ");
						break;
					case 4:
						HD44780_Puts(0,1, (char*)"40mA            ");
						break;
					case 5:
						HD44780_Puts(0,1, (char*)"50mA            ");
						break;
					case 6:
						HD44780_Puts(0,1, (char*)"60mA            ");
						break;
					case 7:
						HD44780_Puts(0,1, (char*)"90mA            ");
						break;
					case 8:
						HD44780_Puts(0,1, (char*)"150mA           ");
						break;
					}

					buzz(800, 100);
					buzz(500, 200);
					buzz(800, 100);
					buzz(900, 300);

					while(Delay_mS(&DelayApp, 1000) == 0);
					HD44780_Puts(0,1, (char*)"                ");
				}
			}

			if(TeclaMais == 1)
			{
				TeclaMais = 0;
				if(menu_index < 8)
				{
					menu_index++;
				}
				else if(menu_index >= 8)
				{
					menu_index = 1;
				}
				BUZ_Play(1, 50,50);
				HD44780_Puts(0,0, (char*)str_menu[menu_index]);
			}

			if(TeclaMenos == 1)
			{
				TeclaMenos = 0;
				PORTE.PODR.BYTE |= (1<<7);
				while(Delay_mS(&DelayApp, 1) == 0);
				PORTE.PODR.BYTE &= ~(1<<7);
			}

			break;
		}

		case 1:
		{
			//Implementar !!
			break;
		}

		case 2:
		{
			//Implementar !!
			break;
		}
	}
}

void TeclaEnter_TestTask(void)
{
	int b = checkButton_Enter();

	if (b == CLICK_EVENT)
		HD44780_Puts(0,1, (char*)"1 Single Click  ");

	if (b == DOUBLE_CLICK_EVENT)
		HD44780_Puts(0,1, (char*)"2 Double Click  ");

	if (b == HOLD_EVENT)
		HD44780_Puts(0,1, (char*)"3 HOLD Click    ");

	if (b == LONG_HOLD_EVENT)
		HD44780_Puts(0,1, (char*)"4 LongHOLDClick ");
}

void TeclaEnter_MainTask(void)
{
	uint8_t reading = TECLA_ENTER;

	if (reading != PinEnterStateOld)
	{
		PinEnterlastDebounceTime = millis();
	}

	if ((millis() - PinEnterlastDebounceTime) > debounceDelay)
	{
		if (reading != PinEnterState)
		{
			PinEnterState = reading;

			if (PinEnterState == 0)
			{
				TeclaEnter = 1;
			}
		}
	}

	PinEnterStateOld = reading;
}

void TeclaMais_MainTask(void)
{
	uint8_t reading = TECLA_MAIS;

	if (reading != PinMaisStateOld)
	{
		PinMaislastDebounceTime = millis();
	}

	if ((millis() - PinMaislastDebounceTime) > debounceDelay)
	{
		if (reading != PinMaisState)
		{
			PinMaisState = reading;

			if (PinMaisState == 0)
			{
				TeclaMais = 1;
			}
		}
	}

	PinMaisStateOld = reading;
}

void TeclaMenos_MainTask(void)
{
	uint8_t reading = TECLA_MENOS;

	if (reading != PinMenosStateOld)
	{
		PinMenoslastDebounceTime = millis();
	}

	if ((millis() - PinMenoslastDebounceTime) > debounceDelay)
	{
		if (reading != PinMenosState)
		{
			PinMenosState = reading;

			if (PinMenosState == 0)
			{
				TeclaMenos = 1;
			}
		}
	}

	PinMenosStateOld = reading;
}

void __delay_ms(uint32_t compare)
{
	uint32_t currentTicks = millis();
	while(millis()- currentTicks < compare);
}

uint32_t millis(void)
{
    return Sys_TickTimer;
}

void buzz(long frequency, long length)
{
	long delayValue = 1000000 / frequency / 2;
	long numCycles = frequency * length / 1000;

	for (long i = 0; i < numCycles; i++)
	{
		Turn_On_Buzzer();
		usb_cpu_delay_1us(delayValue);
		Turn_Off_Buzzer();
		usb_cpu_delay_1us(delayValue);
	}
}

void Sing_MarioTheme(void)
{
	int size = sizeof(melody) / sizeof(int);
	for (int thisNote = 0; thisNote < size; thisNote++)
	{
		int noteDuration = 1000 / tempo[thisNote];
		buzz(melody[thisNote], noteDuration);
		int pauseBetweenNotes = (int)(noteDuration * 1.30);
		__delay_ms(pauseBetweenNotes);
		buzz(0, noteDuration);
	}
}

void R_MAIN_UserInit(void)
{
	EI();
}

void usb_smpl_set_suspend_flag(uint8_t data)
{
  g_usb_suspend_flag  = data;
}

void usb_smpl_led_position(uint8_t ledpos, uint8_t data)
{
#ifdef USB_LED_ENABLE
  usb_cpu_led_set_bit(ledpos, data);
#endif
}

uint8_t usb_smpl_key_input(void)
{
#ifdef USB_KEY_ENABLE
  return  usb_cpu_key_read();
#else
  return  NO_PUSH;                    /* KEY OFF */
#endif
}

void usb_smpl_set_intkey(uint8_t data)
{
  g_usb_int_key_flag  = data;
}

void OpenMsg(uint8_t ptrMsg)
{
  	for (uint16_t i=0;i<256;i++)
  	{
    	msg.MAIN_VECTOR[i] = flash.msg[ptrMsg][i];
  	}
}

void SaveMsg(uint8_t ptrMsg)
{
  	for (uint16_t i=0;i<256;i++)
    	flash.msg[ptrMsg][i] = msg.MAIN_VECTOR[i];

  	FDL_WriteControl();
}

void FDL_Open (void)
{
  	uint32_t          nops_cnt;
  	volatile uint32_t i;

  	FLASH.DFLCTL.BIT.DFLEN = 1;

  	/* wait 5 us */
  	nops_cnt = 5 * g_eel_AppFdlConfig_enu.fdlFrequencyCpuMHz_u16 * 3;

  	for (i = 0; i < nops_cnt; i++)
  	{
    	/* wait */
  	}
}

void FDL_Close (void)
{
  	FLASH.DFLCTL.BIT.DFLEN = 0;
}

void FDL_WriteControl (void)
{
 	 r_fdl_status_t  fdl_ret;
  	r_fdl_request_t req;

  	FDL_Open ();

  	/* 1st initialize the FDL */
  	fdl_ret = R_FDL_Init (&g_eel_AppFdlConfig_enu);

  	if (R_FDL_OK != fdl_ret)
  	{
    	/* Error handler */
    	while (1)
    	{
    	}
  	}

  /**************************************************************************/
  /* Erase all data flash */

  req.command_enu     = R_FDL_CMD_ERASE;
  req.idx_u32         = 0;// pagina;//PRIMEIRO BLOCO QUE SERÁ APAGADO
  req.cnt_u16         = 8;//NÚMERO DE BLOCOS PARA SER ZERADO

  req.accessType_enu  = R_FDL_ACCESS_USER;
  R_FDL_Execute (&req);

  while (R_FDL_BUSY == req.status_enu)
  {
    R_FDL_Handler ();
  }

  if (R_FDL_OK != req.status_enu)
  {
    /* Error handler */
    while (1)
    {
    }
  }

  req.command_enu     = R_FDL_CMD_WRITE;
  req.idx_u32         = 0;
  req.cnt_u16         = sizeof(flash);
  req.dataAdd_u32     = (uint32_t)(&flash.FLASH_VECTOR[0]);
  req.accessType_enu  = R_FDL_ACCESS_USER;
  R_FDL_Execute (&req);


  while (R_FDL_BUSY == req.status_enu)
  {
    R_FDL_Handler ();
  }

  if (R_FDL_OK != req.status_enu)
  {//CASO DÊ ALGUM PAU NA ESCRITA DA MEMÓRIA TRAVA AQUI
    //ESPERO QUE ISSO NUNCA ACONTEÇA...
    /* Error handler */
    while (1)
    {
    }
  }

  FDL_Close ();

}

void FDL_LoadControl (void)
{
  r_fdl_request_t req;
  uint8_t* pointer_dataflash;
  pointer_dataflash = (uint8_t*)(0x00100000);
  r_fdl_status_t  fdl_ret;

  FDL_Open ();

  /* 1st initialize the FDL */
  fdl_ret = R_FDL_Init (&g_eel_AppFdlConfig_enu);

  if (R_FDL_OK != fdl_ret)
  {
    /* Error handler */
    while (1)
    {
    }
  }


  /**************************************************************************/
  /* Blank check on the Data Flash address 0x1c00 ... 0x1fff (last block) */
  req.command_enu     = R_FDL_CMD_BLANKCHECK;
  req.idx_u32         = 0;
  unsigned char effect = 0;
  req.cnt_u16         = sizeof (flash.FLASH_VECTOR[0]);
  req.accessType_enu  = R_FDL_ACCESS_USER;
  R_FDL_Execute (&req);

  while (R_FDL_BUSY == req.status_enu)
  {
    R_FDL_Handler ();
  }

  if (R_FDL_OK == req.status_enu)
  { //CARREGA DAQUI O VALOR PADRÃO, OU NO CASO O QUE FOR SALVO...
    //mensagens padrão
    ERRO_USB = 0;
    const uint8_t RAW_VECTOR_EEPROM[256]  = {      /* Matriz crua, não tratada */
     'M','S','G', 0,  0,  0, 0,  0,//7
      0,  0,  0,  0,  0,  0,  0,  0 ,//17
      0,  0,  0,  0,  0,  0,  0,  0 ,//23
      0,  0,  0,  0,  0,  0,  0,  0 ,//31
      0,  0,  0,  0,  0,  0,  0,  0 ,//39
      0,  0,  0,  0,  0,  0,  0,  0 ,//47
      0,  0,  0,  0,  0,  0,  0,  0 ,//55
      0,  0,  0,  0,  0,  0,  0,  0 ,//63
      0,  0,  0,  0,  0,  0,  0,  0 ,//71
      0,  0,  0,  0,  0,  0,  0,  0 ,//79
      0,  0,  0,  0,  0,  0,  0,  0 ,//87
      0,  0,  0,  0,  0,  0,  0,  0 ,//95
      0,  0,  0,  0,  0,  0,  0,  0 ,//103
      0,  0,  0,  0,  0,  0,  0,  0 ,//111
      0,  0,  0,  0,  0,  0,  0,  0 ,//119
      0,  0,  0,  0,  0,  0,  0,  0 ,//127
      0,  0,  0,  0,  0,  0,  0,  0 ,//135
      0,  0,  0,  0,  0,  0,  0,  0 ,//143
      0,  0,  0,  0,  0,  0,  0,  0 ,//151
      0,  0,  0,  0,  0,  0,  0,  0 ,//169
      0,  0,  0,  0,  0,  0,  0,  0 ,//167
      0,  0,  0,  0,  0,  0,  0,  0 ,//175
      0,  0,  0,  0,  0,  0,  0,  0 ,//183
      0,  0,  0,  0,  0,  0,  0,  0 ,//191
      0,  0,  0,  0,  0,  0,  0,  0 ,//199
      0,  0,  0,  0,  0,  0,  0,  0 ,//207
      0,  0,  0,  0,  0,  0,  0,  0 ,//215
      0,  0,  0,  0,  0,  0,  0,  0 ,//223
      0,  0,  0,  0,  0,  0,  0,  0 ,//231
      0,  0,  0,  0,  0,  0,  0,  0 ,//239
      0,  2,  19,  0, 170,0,  0,  0 ,//247//PARTE DOS EFEITOS E DAS MSGS EXTRAS, COM TEMP EXT DESABALITADA NO INICIO
      //EFEITO, HABILITADA, VELOCIDADE
      0, 15,  0,  3,  104,  0,  0,  0 ,//255//BYTES RESERVA
    };
    uint16_t i=0;
    for(uint16_t j=0;j<sizeof(flash.FLASH_VECTOR);j++)
    {
          if (effect < 9)
          {
            if (i == 3)
              flash.FLASH_VECTOR[j] = effect+'1';
            else
            {
              if (i==241)
                if(effect<6)//habilita as seis primeiras
                  flash.FLASH_VECTOR[j] = 2;
                else
                  flash.FLASH_VECTOR[j] = 0;
              else
                flash.FLASH_VECTOR[j]=RAW_VECTOR_EEPROM[i];
            }
          }
          else
          {
            if (i == 3)
              flash.FLASH_VECTOR[j] = '1';
            else if (i == 4)
              flash.FLASH_VECTOR[j] = effect+'0'-9;
            else
            {
              if (i==241)
                  flash.FLASH_VECTOR[j] = 0;
              else
                flash.FLASH_VECTOR[j]=RAW_VECTOR_EEPROM[i];
            }
          }
      i++;
      if (i>255)
      {
        i=0;
        effect++;
      }
    }
    FDL_WriteControl();
  }
  else
  {
    for(uint16_t i=0;i<sizeof(flash.FLASH_VECTOR);i++)
    {
        flash.FLASH_VECTOR[i] = *pointer_dataflash;
        pointer_dataflash++;
    }
    FDL_Close ();
  }

  return;
}

void Bitmap_Sample1(void)
{
	// bitmap.line sample #1
	HD44780_Puts(0,1, (char*)".line #1 ");
	LCDBitmap_clear();  // Clear the bitmap display (automatically updates bitmap display) doesn't clear text
	uint32_t currentMillis = millis();
	uint8_t startdelay = 120;
	while(millis()-currentMillis<sample*2)
	{
		for (uint8_t x=0; x<BITMAP_W; x=x+2)
		{
			LCDBitmap_line(x, 0, BITMAP_W-x-1, BITMAP_H-1, ON, UPDATE);  // Draw the line from (x, 0) to (BITMAP_W-x-1, BITMAP_H-1) and update bitmap display
			__delay_ms(startdelay);
			LCDBitmap_clear();  // Clear the display
		}
		for (uint8_t y=0; y<BITMAP_H; y=y+2)
		{
			LCDBitmap_line(0, BITMAP_H-y-1, BITMAP_W-1, y, ON, UPDATE);
			__delay_ms(startdelay);
			LCDBitmap_clear();  // Clear the display
		}
		startdelay = startdelay * 2 / 3;
	}
}

void Bitmap_Sample2(void)
{
	// bitmap.line sample #2
	HD44780_Puts(0,1, (char*)".line #2 ");
	uint32_t currentMillis = millis();

	while(millis()-currentMillis<sample)
	{
		for (uint8_t x=0; x<BITMAP_W; x++)
		{
			LCDBitmap_line(x, 0, x, BITMAP_H-1, ON, UPDATE);  // Draw the line from (x, 0) to (x, BITMAP_H-1) and update bitmap display
		}

		for (uint8_t y=0; y<BITMAP_H; y++)
		{
			LCDBitmap_line(0, y, BITMAP_W-1, y, OFF, UPDATE);
		}
	}
	LCDBitmap_clear();  // Clear the display
}

void Bitmap_Sample3(void)
{
	// This creates a big "X" using the LCDBitmap to show the inverse and move functions
	for (uint8_t x=0; x<=4; x++)
	{
		LCDBitmap_line(x, BITMAP_H-1, x+BITMAP_H-1, 0, ON, NO_UPDATE);  // Draw a line, but don't update the bitmap display
		LCDBitmap_line(x, 0, x+BITMAP_H-1, BITMAP_H-1, ON, NO_UPDATE);  // Draw a line, but don't update the bitmap display
	}

	LCDBitmap_update();  // Now that everything is written to the bitmap, show the changes

	__delay_ms(1000);
}

void Sing_MarchaImperial(void)
{
    buzz(_a, 500);
    buzz(_a, 500);
    buzz(_a, 500);
    buzz(_f, 350);
    buzz(_cH, 150);

    buzz(_a, 500);
    buzz(_f, 350);
    buzz(_cH, 150);
    buzz(_a, 1000);
    //first bit

    buzz(_eH, 500);
    buzz(_eH, 500);
    buzz(_eH, 500);
    buzz(_fH, 350);
    buzz(_cH, 150);

    buzz(_gS, 500);
    buzz(_f, 350);
    buzz(_cH, 150);
    buzz(_a, 1000);
    //second bit...

    buzz(_aH, 500);
    buzz(_a, 350);
    buzz(_a, 150);
    buzz(_aH, 500);
    buzz(_gSH, 250);
    buzz(_gH, 250);

    buzz(_fSH, 125);
    buzz(_fH, 125);
    buzz(_fSH, 250);
    __delay_ms(250);
    buzz(_aS, 250);
    buzz(_dSH, 500);
    buzz(_dH, 250);
    buzz(_cSH, 250);
    //start of the interesting bit

    buzz(_cH, 125);
    buzz(_b, 125);
    buzz(_cH, 250);
    __delay_ms(250);
    buzz(_f, 125);
    buzz(_gS, 500);
    buzz(_f, 375);
    buzz(_a, 125);

    buzz(_cH, 500);
    buzz(_a, 375);
    buzz(_cH, 125);
    buzz(_eH, 1000);
    //more interesting stuff (this doesn't quite get it right somehow)

    buzz(_aH, 500);
    buzz(_a, 350);
    buzz(_a, 150);
    buzz(_aH, 500);
    buzz(_gSH, 250);
    buzz(_gH, 250);

    buzz(_fSH, 125);
    buzz(_fH, 125);
    buzz(_fSH, 250);
    __delay_ms(250);
    buzz(_aS, 250);
    buzz(_dSH, 500);
    buzz(_dH, 250);
    buzz(_cSH, 250);
    //repeat... repeat

    buzz(_cH, 125);
    buzz(_b, 125);
    buzz(_cH, 250);
    __delay_ms(250);
    buzz(_f, 250);
    buzz(_gS, 500);
    buzz(_f, 375);
    buzz(_cH, 125);

    buzz(_a, 500);
    buzz(_f, 375);
    buzz(_c, 125);
    buzz(_a, 1000);
}



/* End user code. Do not edit comment generated here */
