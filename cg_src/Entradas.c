/* Includes --------------------------------------------------------------------------------------*/
#include "Entradas.h"
#include "r_user_hwrctrl.h"


/* Definições-------------------------------------------------------------------------------------*/
#define INPUT_VECTOR_SIZE		32


/* Variaveis -------------------------------------------------------------------------------------*/
uint16_t    adc_value_raw;
uint16_t	adc_value_filter = 0;
uint16_t 	adc_value_vector[ INPUT_VECTOR_SIZE ];
uint8_t 	adc_value_index;
uint8_t     g_adc_complete;
uint16_t	DelayAdc;

// number of analog samples to take per reading
#define NUM_SAMPLES 10

int sum = 0;                    // sum of samples taken
unsigned char sample_count = 0; // current sample number
float voltage = 0.0;            // calculated voltage

#define SHUNT_RESISTOR  10		// Shunt resistor value (in ohms)
#define VOLTAGE_REF 	3.3		// Reference voltage for analog read

float ShuntVoltage;   			// Variable to store value from analog read
uint8_t ShuntCurrent;       		// Calculated current value

void ADC_MainTask(void)
{
	if(Delay_mS(&DelayAdc, 5)) //a cada 5ms realiza leitura
	{
		adc_value_raw = ADC_GetValue(ADCHANNEL11);

		// Remap the ADC value into a voltage number (3V3 reference)
		ShuntVoltage = (adc_value_raw * VOLTAGE_REF) / 4096;

		// determine the current flowing through ShuntResistor.
		ShuntCurrent = (uint8_t)(ShuntVoltage * 10);
	}
}

uint16_t ADC_GetValue(ad_channel_t channel)
{
	// A variable to retrieve the adc result
	uint16_t retval;

	// Start a conversion
	R_S12AD_Start();

	// Wait for the A/D conversion to complete
	while (g_adc_complete == 0); //set on interrupt

	// Clear ADC flag
	g_adc_complete = 0;

	//le valor do canal
	R_S12AD_Get_ValueResult(channel, &retval);

	// Stop the A/D converter being triggered from the pin ADTRG0n
	R_S12AD_Stop();

	return retval;
}

uint16_t average_calc( uint16_t *vector_ptr, uint8_t vector_size )
{
	uint16_t acc = 0; // acumulador

	for( uint8_t i = 0; i < vector_size; i++ )
	{
		acc += *vector_ptr++; // soma todos valores lidos
	}

	acc /= vector_size; // calcula media movel do sinal
	return (uint16_t)( acc );
}

/*****END OF FILE****/
