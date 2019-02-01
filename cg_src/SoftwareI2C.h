#ifndef _SOFTWAREI2C_H_
#define _SOFTWAREI2C_H_

#include "r_cg_macrodriver.h"

#define  GETACK        1                      // get ack
#define  GETNAK        0                      // get nak

#ifndef  HIGH
#define  HIGH          1
#endif

#ifndef  LOW
#define  LOW           0
#endif

#ifndef  OUTPUT
#define  OUTPUT        1
#endif

#ifndef  INPUT
#define  INPUT        0
#endif

void SoftwareI2C_Init(void);
void SoftwareI2C_sdaSet(uint8_t ucDta);
void SoftwareI2C_sclSet(uint8_t ucDta);
void SoftwareI2C_sendStart(void);
void SoftwareI2C_sendStop(void);
uint8_t SoftwareI2C_getAck(void);
void SoftwareI2C_sendByte(uint8_t ucDta);
uint8_t SoftwareI2C_sendByteAck(uint8_t ucDta); // send byte and get ack
uint8_t SoftwareI2C_beginTransmission(uint8_t addr);
uint8_t SoftwareI2C_endTransmission();
uint8_t SoftwareI2C_write(uint8_t dta);
uint8_t SoftwareI2C_requestFrom(uint8_t addr, uint8_t len);
uint8_t SoftwareI2C_read();
uint8_t SoftwareI2C_available(void);
void SoftwareI2C_usDelay(uint32_t x);
void softI2C_write_bit(uint8_t x, uint8_t m);
void softI2C_write_uint8(uint8_t x);

#endif

