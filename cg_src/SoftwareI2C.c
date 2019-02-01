#include "SoftwareI2C.h"
#include "r_cg_main.h"

uint8_t recv_len;
uint8_t sda_in_out;

#define SDA_PIN		PORT1.PODR.BIT.B7
#define SCL_PIN		PORT1.PODR.BIT.B6
#define I2C_DELAY	1

void SoftwareI2C_usDelay(uint32_t x)
{
	//__delay_ms(x);
    for( uint16_t i = 0; i < (3 * x); ++i );
}

void SoftwareI2C_Init(void)
{
    sda_in_out = OUTPUT;
	P17_Cfg_Output(); //SDA
	P16_Cfg_Output(); //SCL
	SDA_PIN = HIGH;
	SCL_PIN = HIGH;
}

uint8_t SoftwareI2C_available(void)
{
	return recv_len;
}

void SoftwareI2C_sdaSet(uint8_t ucDta)
{
    if(sda_in_out != OUTPUT)
    {
        sda_in_out = OUTPUT;
        SDA_Cfg_Output();
    }
    SDA_PIN = ucDta;
}

void SoftwareI2C_sclSet(uint8_t ucDta)
{
    SCL_PIN = ucDta;
}

uint8_t SoftwareI2C_getAck(void)
{
    SoftwareI2C_sclSet(LOW);
    SDA_Cfg_Input();
    sda_in_out = INPUT;

    SoftwareI2C_sclSet(HIGH);
    unsigned long timer_t = millis();
    while(1)
    {
        if(!PORT1.PIDR.BIT.B7) // get ack
        {
            return GETACK;
        }

        if(millis() - timer_t > 100)
			return GETNAK;
    }
}

void SoftwareI2C_sendStart(void)
{
	SoftwareI2C_sclSet(HIGH);
	SoftwareI2C_usDelay(I2C_DELAY);

    SoftwareI2C_sdaSet(LOW);
	SoftwareI2C_usDelay(I2C_DELAY);
}

void SoftwareI2C_sendStop(void)
{
    SoftwareI2C_sdaSet(LOW);
	SoftwareI2C_usDelay(I2C_DELAY);

    SoftwareI2C_sclSet(HIGH);
	SoftwareI2C_usDelay(I2C_DELAY);

    SoftwareI2C_sdaSet(HIGH);
	SoftwareI2C_usDelay(I2C_DELAY);
}

void SoftwareI2C_sendByte(uint8_t ucDta)
{
	for(uint8_t i=0; i<8; i++)
    {
        SoftwareI2C_sclSet(LOW);
		SoftwareI2C_usDelay(I2C_DELAY);

		if(ucDta & 0x80)
			SoftwareI2C_sdaSet(HIGH);
		else
			SoftwareI2C_sdaSet(LOW);

		SoftwareI2C_usDelay(I2C_DELAY);
        SoftwareI2C_sclSet(HIGH);
		SoftwareI2C_usDelay(I2C_DELAY);
        ucDta <<= 1;
    }
}

uint8_t SoftwareI2C_sendByteAck(uint8_t ucDta)
{
    SoftwareI2C_sendByte(ucDta);
    return SoftwareI2C_getAck();
}

uint8_t SoftwareI2C_beginTransmission(uint8_t addr)
{
    SoftwareI2C_sendStart();                            // start signal
    uint8_t ret = SoftwareI2C_sendByteAck(addr<<1);       // send write address and get ack
    return ret;
}

uint8_t SoftwareI2C_endTransmission(void)
{
    SoftwareI2C_sendStop();
    return 0;
}

uint8_t SoftwareI2C_write(uint8_t dta)
{
    return SoftwareI2C_sendByteAck(dta);
}

uint8_t SoftwareI2C_requestFrom(uint8_t addr, uint8_t len)
{
    SoftwareI2C_sendStart();                       // start signal
    recv_len = len;
    uint8_t ret = SoftwareI2C_sendByteAck((addr<<1)+1);       // send write address and get ack
    return ret;
}

uint8_t SoftwareI2C_read(void)
{
    if(!recv_len)
		return 0;

    uint8_t ucRt = 0;

   	SDA_Cfg_Input();
    sda_in_out = INPUT;

    for(int i=0; i<8; i++)
    {
        unsigned  char  ucBit;
        SoftwareI2C_sclSet(LOW);
        SoftwareI2C_sclSet(HIGH);
        ucBit = PORT1.PIDR.BIT.B7;
        ucRt = (ucRt << 1) + ucBit;
    }

    uint8_t dta = ucRt;
    recv_len--;

    if(recv_len>0) // send ACK
    {
        SoftwareI2C_sclSet(LOW);
        SoftwareI2C_sdaSet(LOW);
        SoftwareI2C_sclSet(HIGH);
        SoftwareI2C_sclSet(LOW);
    }
    else // send NAK
    {
        SoftwareI2C_sclSet(LOW);
        SoftwareI2C_sdaSet(HIGH);
        SoftwareI2C_sclSet(HIGH);
        SoftwareI2C_sclSet(LOW);
        SoftwareI2C_sendStop();
    }
    return dta;
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/