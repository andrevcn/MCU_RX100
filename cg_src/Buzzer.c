/* Inclusoes de arquivos de cabecalho ----------------------------------------*/
#include "Buzzer.h"

/*******************************************************************************
Exemplo de uso deste driver...

	//Exemplo1:
	#include "led.h"

	//Variavel de controle do LED/Pino
	Led_t led1;

	void main(void)
	{
		//Inicializa Pino e Variaveis do LED
		LED_PinInit(&led1, PB0);

		//Pisca LED1 3 vezes, sendo 1 segundo ligado e 1 segundo desligado.
		LED_Play(&led1, 3, 1000, 1000);

		while(1);
	}

	void SysTick_Handler(void) //@1ms
	{
		LED_InterruptHandle();
	}

	////////////////////////////////////////////////////////////////////////////

	//Exemplo2:
	#include "led.h"

	//Variavel de controle do LED/Pino
	Led_t led1;
	Led_t led2;

	void main(void)
	{
		//Inicializa Pino e Variaveis do LED
		LED_SetUp();

		//Pisca LED1 3 vezes, sendo 1 segundo ligado e 1 segundo desligado.
		LED_Play(&led1, 3, 1000, 1000);

		//Pisca LED2 5 vezes, sendo 50ms ligado e 350ms desligado.
		LED_Play(&led2, 5, 50, 350);

		while(1);
	}

	void SysTick_Handler(void) //@1ms
	{
		LED_InterruptHandle();
	}

*******************************************************************************/

/* Variaveis privadas --------------------------------------------------------*/
//Led_t led1;
uint8_t 	Times;
uint16_t 	TimeOn;
uint16_t 	TimeOff;
uint16_t 	CounterOn;
uint16_t 	CounterOff;


/**
 * @brief  Inicializa/Assinala o o pino ao led structure
 * @param  Led: Estrutura que vai controlar o led
 * @param  pin: Pino que vai controlar o led
 * @retval None
 */
void BUZ_PinInit(void)
{
	Times      = 0;
	TimeOn     = 0;
	TimeOff    = 0;
	CounterOn  = 0;
	CounterOff = 0;
}

/**
 * @brief  Seta o led structure
 * @param  Led: Estrutura do led/pin
 * @param  times: quantas vezes será acionado/ligado
 * @param  time_on: tempo que ficará ligado
 * @param  time_off: tempo que ficará desligado
 * @retval None
 */
void BUZ_Play(uint8_t times, uint16_t time_on, uint16_t time_off)
{
    Times      = times;
    TimeOn     = time_on;
    TimeOff    = time_off;
    CounterOn  = TimeOn;
    CounterOff = TimeOff;
}

/**
 * @brief
 * @param  Led: Estrutura do led/pin
 * @param  Active: Desliga led structure on interrupt
 * @retval None
 */
void BUZ_Stop(void)
{
    Times  = 0;
    Turn_Off_Buzzer();
}

/**
 * @brief  Processa/controla o tempo/gpio do led structure
 * @param  Led: Estrutura que vai controlar o led/pino
 * @retval None
 */
void BUZ_ISR_Task(void)
{
	if(Times > 0)
	{
		if(CounterOn > 0)
		{
			Turn_On_Buzzer();
			CounterOn--;
		}
		else
		{
			Turn_Off_Buzzer();
			if(!CounterOff)
			{
				Times--;
				CounterOn  = TimeOn;
				CounterOff = TimeOff;
			}
			else
				CounterOff--;
		}
	}
}





/** (C) COPYRIGHT 2018   ARTEC TECNOLOGIA ELETRONICA. **/
/*****END OF FILE****/


