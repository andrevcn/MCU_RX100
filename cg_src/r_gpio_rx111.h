#ifndef GPIO_RX111
#define GPIO_RX111

#include "r_cg_macrodriver.h"

/* General information about number of ports and pins on this device. */
#define GPIO_INFO_NUM_PORTS             (19) //Range from first to last port. Useful for arrays.
#define GPIO_INFO_NUM_PINS              (36)


/* For testing we will allocate virtual IO ports. */
#if !defined(GPIO_TESTING)
/* Base registers used for offsets on output data registers. */
#define     GPIO_PRV_BASE_ADDR_OUTPUT           ((uint8_t volatile *)&PORT0.PODR.BYTE)
/* Base registers used for offsets on input data registers. */
#define     GPIO_PRV_BASE_ADDR_INPUT            ((uint8_t volatile *)&PORT0.PIDR.BYTE)
/* Base registers used for offsets on direction registers. */
#define     GPIO_PRV_BASE_ADDR_DIRECTION        ((uint8_t volatile *)&PORT0.PDR.BYTE)
/* Base registers used for offsets on mode registers. */
#define     GPIO_PRV_BASE_ADDR_MODE             ((uint8_t volatile *)&PORT0.PMR.BYTE)
/* Base registers used for offsets on output type registers. */
#define     GPIO_PRV_BASE_ADDR_OUT_TYPE         ((uint8_t volatile *)&PORT3.ODR0.BYTE-6)
/* Base registers used for offsets on pull-up registers. */
#define     GPIO_PRV_BASE_ADDR_PULL_UP          ((uint8_t volatile *)&PORT0.PCR.BYTE)
#endif




/* This enumerator has each available GPIO port on this MCU. This list will change depending on the MCU chosen. */
typedef enum{
    GPIO_PORT_1 = 0x0100,
    GPIO_PORT_2 = 0x0200,
    GPIO_PORT_3 = 0x0300,
    GPIO_PORT_4 = 0x0400,
    GPIO_PORT_A = 0x0A00,
    GPIO_PORT_B = 0x0B00,
    GPIO_PORT_C = 0x0C00,
    GPIO_PORT_E = 0x0E00,
    GPIO_PORT_H = 0x1100,
    GPIO_PORT_J = 0x1200,
} gpio_port_t;

/* This enumerator has a bit mask for each available GPIO pin for the given port on this MCU. */
typedef enum{
    GPIO_PORT0_PIN_MASK = 0x00,    /* Available pins: None               */
    GPIO_PORT1_PIN_MASK = 0xF0,    /* Available pins: P14, P15, P16,P17  */
    GPIO_PORT2_PIN_MASK = 0xC0,    /* Available pins: P26,P27            */
    GPIO_PORT3_PIN_MASK = 0x20,    /* Available pins: P35                */
    GPIO_PORT4_PIN_MASK = 0x47,    /* Available pins: P40 to P42, P46    */
    GPIO_PORT5_PIN_MASK = 0x00,    /* Available pins: None               */
    GPIO_PORTA_PIN_MASK = 0x5A,    /* Available pins: PA1, PA3, PA4, PA6 */
    GPIO_PORTB_PIN_MASK = 0x2B,    /* Available pins: PB0, PB1, PB3, PB5 */
    GPIO_PORTC_PIN_MASK = 0xFF,    /* Available pins: PC0 to PC7         */
    GPIO_PORTE_PIN_MASK = 0x9F,    /* Available pins: PE0 to PE4, PE7    */
    GPIO_PORTH_PIN_MASK = 0x80,    /* Available pins: PH7                */
    GPIO_PORTJ_PIN_MASK = 0xC0,    /* Available pins: PJ6, PJ7           */
} gpio_pin_bit_mask_t;

typedef enum{
    GPIO_PORT_1_PIN_4 = 0x0104,
    GPIO_PORT_1_PIN_5 = 0x0105,
    GPIO_PORT_1_PIN_6 = 0x0106,
    GPIO_PORT_1_PIN_7 = 0x0107,
    GPIO_PORT_2_PIN_6 = 0x0206,
    GPIO_PORT_2_PIN_7 = 0x0207,
    GPIO_PORT_3_PIN_5 = 0x0305,
    GPIO_PORT_4_PIN_0 = 0x0400,
    GPIO_PORT_4_PIN_1 = 0x0401,
    GPIO_PORT_4_PIN_2 = 0x0402,
    GPIO_PORT_4_PIN_6 = 0x0406,
    GPIO_PORT_A_PIN_1 = 0x0A01,
    GPIO_PORT_A_PIN_3 = 0x0A03,
    GPIO_PORT_A_PIN_4 = 0x0A04,
    GPIO_PORT_A_PIN_6 = 0x0A06,
    GPIO_PORT_B_PIN_0 = 0x0B00,
    GPIO_PORT_B_PIN_1 = 0x0B01,
    GPIO_PORT_B_PIN_3 = 0x0B03,
    GPIO_PORT_B_PIN_5 = 0x0B05,
    GPIO_PORT_C_PIN_0 = 0x0C00,
    GPIO_PORT_C_PIN_1 = 0x0C01,
    GPIO_PORT_C_PIN_2 = 0x0C02,
    GPIO_PORT_C_PIN_3 = 0x0C03,
    GPIO_PORT_C_PIN_4 = 0x0C04,
    GPIO_PORT_C_PIN_5 = 0x0C05,
    GPIO_PORT_C_PIN_6 = 0x0C06,
    GPIO_PORT_C_PIN_7 = 0x0C07,
    GPIO_PORT_E_PIN_0 = 0x0E00,
    GPIO_PORT_E_PIN_1 = 0x0E01,
    GPIO_PORT_E_PIN_2 = 0x0E02,
    GPIO_PORT_E_PIN_3 = 0x0E03,
    GPIO_PORT_E_PIN_4 = 0x0E04,
    GPIO_PORT_E_PIN_7 = 0x0E07,
    GPIO_PORT_H_PIN_7 = 0x1107,
    GPIO_PORT_J_PIN_6 = 0x1206,
    GPIO_PORT_J_PIN_7 = 0x1207,
} gpio_port_pin_t;

typedef enum{
    GPIO_PIN_OUT_CMOS              = 0,
    GPIO_PIN_OUT_OPEN_DRAIN_N_CHAN = 1,
    GPIO_PIN_OUT_OPEN_DRAIN_P_CHAN = 2
} gpio_pin_output_t;

/* Levels that can be set and read for individual pins. */
typedef enum{
    GPIO_LEVEL_LOW = 0,
    GPIO_LEVEL_HIGH
} gpio_level_t;

/* Options that can be used with the R_GPIO_PortDirectionSet() and R_GPIO_PinDirectionSet() functions. */
typedef enum{
    GPIO_DIRECTION_INPUT = 0,
    GPIO_DIRECTION_OUTPUT
} gpio_dir_t;

/* Commands that can be used with the R_GPIO_PinControl() function. This list will vary depending on the MCU chosen. */
typedef enum{
    GPIO_CMD_OUT_CMOS = 0,
    GPIO_CMD_OUT_OPEN_DRAIN_N_CHAN,
    GPIO_CMD_OUT_OPEN_DRAIN_P_CHAN,
    GPIO_CMD_IN_PULL_UP_DISABLE,
    GPIO_CMD_IN_PULL_UP_ENABLE,
    GPIO_CMD_ASSIGN_TO_PERIPHERAL,
    GPIO_CMD_ASSIGN_TO_GPIO,
    GPIO_CMD_DSCR_DISABLE,
    GPIO_CMD_DSCR_ENABLE,
    GPIO_CMD_DSCR2_DISABLE,
    GPIO_CMD_DSCR2_ENABLE
} gpio_cmd_t;

/* Function return type. */
typedef enum{
    GPIO_SUCCESS = 0,
    GPIO_ERR_INVALID_MODE,  // The mode specified cannot be applied to this pin
    GPIO_ERR_INVALID_CMD    // The input command is not supported
} gpio_err_t;


/* Prototipos de funções */
void R_GPIO_PortWrite (gpio_port_t port, uint8_t value);
uint8_t R_GPIO_PortRead (gpio_port_t port);
void R_GPIO_PortDirectionSet (gpio_port_t port, gpio_dir_t dir, uint8_t mask);
void R_GPIO_PinWrite (gpio_port_pin_t pin, gpio_level_t level);
gpio_level_t R_GPIO_PinRead (gpio_port_pin_t pin);
void R_GPIO_PinDirectionSet (gpio_port_pin_t pin, gpio_dir_t dir);
uint8_t volatile * gpio_port_addr_get (uint8_t volatile * base_addr, uint16_t index);
void gpio_set_output_type (gpio_port_pin_t pin, gpio_pin_output_t out_type);
gpio_err_t R_GPIO_PinControl (gpio_port_pin_t pin, gpio_cmd_t cmd);
void digitalWrite(gpio_port_pin_t pin, uint8_t level);

#endif /* GPIO_RX111 */
