#ifndef PORT_H
#define PORT_H

/*
    Port Direction Register (PDR)
*/
/* Pmn Direction Control (B7 - B0) */
#define _00_Pm0_MODE_NOT_USED   (0x00U) /* Pm0 not used */
#define _00_Pm0_MODE_INPUT      (0x00U) /* Pm0 as input */
#define _01_Pm0_MODE_OUTPUT     (0x01U) /* Pm0 as output */
#define _00_Pm1_MODE_NOT_USED   (0x00U) /* Pm1 not used */
#define _00_Pm1_MODE_INPUT      (0x00U) /* Pm1 as input */
#define _02_Pm1_MODE_OUTPUT     (0x02U) /* Pm1 as output */
#define _00_Pm2_MODE_NOT_USED   (0x00U) /* Pm2 not used */
#define _00_Pm2_MODE_INPUT      (0x00U) /* Pm2 as input */
#define _04_Pm2_MODE_OUTPUT     (0x04U) /* Pm2 as output */
#define _00_Pm3_MODE_NOT_USED   (0x00U) /* Pm3 not used */
#define _00_Pm3_MODE_INPUT      (0x00U) /* Pm3 as input */
#define _08_Pm3_MODE_OUTPUT     (0x08U) /* Pm3 as output */
#define _00_Pm4_MODE_NOT_USED   (0x00U) /* Pm4 not used */
#define _00_Pm4_MODE_INPUT      (0x00U) /* Pm4 as input */
#define _10_Pm4_MODE_OUTPUT     (0x10U) /* Pm4 as output */
#define _00_Pm5_MODE_NOT_USED   (0x00U) /* Pm5 not used */
#define _00_Pm5_MODE_INPUT      (0x00U) /* Pm5 as input */
#define _20_Pm5_MODE_OUTPUT     (0x20U) /* Pm5 as output */
#define _00_Pm6_MODE_NOT_USED   (0x00U) /* Pm6 not used */
#define _00_Pm6_MODE_INPUT      (0x00U) /* Pm6 as input */
#define _40_Pm6_MODE_OUTPUT     (0x40U) /* Pm6 as output */
#define _00_Pm7_MODE_NOT_USED   (0x00U) /* Pm7 not used */
#define _00_Pm7_MODE_INPUT      (0x00U) /* Pm7 as input */
#define _80_Pm7_MODE_OUTPUT     (0x80U) /* Pm7 as output */

/*
    Port Output Data Register (PODR)
*/
/* Pmn Output Data Store (B7 - B0) */
#define _00_Pm0_OUTPUT_0        (0x00U) /* Output low at B0 */
#define _01_Pm0_OUTPUT_1        (0x01U) /* Output high at B0 */
#define _00_Pm1_OUTPUT_0        (0x00U) /* Output low at B1 */
#define _02_Pm1_OUTPUT_1        (0x02U) /* Output high at B1 */
#define _00_Pm2_OUTPUT_0        (0x00U) /* Output low at B2 */
#define _04_Pm2_OUTPUT_1        (0x04U) /* Output high at B2 */
#define _00_Pm3_OUTPUT_0        (0x00U) /* Output low at B3 */
#define _08_Pm3_OUTPUT_1        (0x08U) /* Output high at B3 */
#define _00_Pm4_OUTPUT_0        (0x00U) /* Output low at B4 */
#define _10_Pm4_OUTPUT_1        (0x10U) /* Output high at B4 */
#define _00_Pm5_OUTPUT_0        (0x00U) /* Output low at B5 */
#define _20_Pm5_OUTPUT_1        (0x20U) /* Output high at B5 */
#define _00_Pm6_OUTPUT_0        (0x00U) /* Output low at B6 */
#define _40_Pm6_OUTPUT_1        (0x40U) /* Output high at B6 */
#define _00_Pm7_OUTPUT_0        (0x00U) /* Output low at B7 */
#define _80_Pm7_OUTPUT_1        (0x80U) /* Output high at B7 */

/*
    Open Drain Control Register 0 (ODR0)
*/
/* Pmn Output Type Select (Pm0 to Pm3) */
#define _00_Pm0_CMOS_OUTPUT     (0x00U) /* CMOS output */
#define _01_Pm0_NCH_OPEN_DRAIN  (0x01U) /* N-channel open-drain output */
#define _00_Pm1_CMOS_OUTPUT     (0x00U) /* CMOS output */
#define _04_Pm1_NCH_OPEN_DRAIN  (0x04U) /* N-channel open-drain output */
#define _08_Pm1_PCH_OPEN_DRAIN  (0x08U) /* P-channel open-drain output, for PE1 only */
#define _00_Pm2_CMOS_OUTPUT     (0x00U) /* CMOS output */
#define _10_Pm2_NCH_OPEN_DRAIN  (0x10U) /* N-channel open-drain output */
#define _00_Pm3_CMOS_OUTPUT     (0x00U) /* CMOS output */
#define _40_Pm3_NCH_OPEN_DRAIN  (0x40U) /* N-channel open-drain output */

/*
    Open Drain Control Register 1 (ODR1)
*/
/* Pmn Output Type Select (Pm4 to Pm7) */
#define _00_Pm4_CMOS_OUTPUT     (0x00U) /* CMOS output */
#define _01_Pm4_NCH_OPEN_DRAIN  (0x01U) /* N-channel open-drain output */
#define _02_Pm4_PCH_OPEN_DRAIN  (0x02U) /* P-channel open-drain output, for P14 only */
#define _00_Pm5_CMOS_OUTPUT     (0x00U) /* CMOS output */
#define _04_Pm5_NCH_OPEN_DRAIN  (0x04U) /* N-channel open-drain output */
#define _00_Pm6_CMOS_OUTPUT     (0x00U) /* CMOS output */
#define _10_Pm6_NCH_OPEN_DRAIN  (0x10U) /* N-channel open-drain output */
#define _00_Pm7_CMOS_OUTPUT     (0x00U) /* CMOS output */
#define _40_Pm7_NCH_OPEN_DRAIN  (0x40U) /* N-channel open-drain output */

/*
    Pull-Up Control Register (PCR)
*/
/* Pm0 Input Pull-Up Resistor Control ((B7 - B0)) */
#define _00_Pm0_PULLUP_OFF      (0x00U) /* Pn0 pull-up resistor not connected */
#define _01_Pm0_PULLUP_ON       (0x01U) /* Pn0 pull-up resistor connected */
#define _00_Pm1_PULLUP_OFF      (0x00U) /* Pn1 pull-up resistor not connected */
#define _02_Pm1_PULLUP_ON       (0x02U) /* Pn1 pull-up resistor connected */
#define _00_Pm2_PULLUP_OFF      (0x00U) /* Pn2 Pull-up resistor not connected */
#define _04_Pm2_PULLUP_ON       (0x04U) /* Pn2 pull-up resistor connected */
#define _00_Pm3_PULLUP_OFF      (0x00U) /* Pn3 pull-up resistor not connected */
#define _08_Pm3_PULLUP_ON       (0x08U) /* Pn3 pull-up resistor connected */
#define _00_Pm4_PULLUP_OFF      (0x00U) /* Pn4 pull-up resistor not connected */
#define _10_Pm4_PULLUP_ON       (0x10U) /* Pn4 pull-up resistor connected */
#define _00_Pm5_PULLUP_OFF      (0x00U) /* Pn5 pull-up resistor not connected */
#define _20_Pm5_PULLUP_ON       (0x20U) /* Pn5 pull-up resistor connected */
#define _00_Pm6_PULLUP_OFF      (0x00U) /* Pn6 pull-up resistor not connected */
#define _40_Pm6_PULLUP_ON       (0x40U) /* Pn6 pull-up resistor connected */
#define _00_Pm7_PULLUP_OFF      (0x00U) /* Pn7 pull-up resistor not connected */
#define _80_Pm7_PULLUP_ON       (0x80U) /* Pn7 pull-up resistor connected */

/*
    Port Switching Register B (PSRB)
*/
/* PB0/PC0 Switching (PSEL0) */
#define _00_PORT_PSEL0_PB0      (0x00U) /* PB0 general I/O port function is selected */
#define _01_PORT_PSEL0_PC0      (0x01U) /* PC0 general I/O port function is selected */
/* PB1/PC1 Switching (PSEL1) */
#define _00_PORT_PSEL1_PB1      (0x00U) /* PB1 general I/O port function is selected */
#define _02_PORT_PSEL1_PC1      (0x02U) /* PC1 general I/O port function is selected */
/* PB3/PC2 Switching (PSEL3) */
#define _00_PORT_PSEL3_PB3      (0x00U) /* PB3 general I/O port function is selected */
#define _08_PORT_PSEL3_PC2      (0x08U) /* PC2 general I/O port function is selected */
/* PB5/PC3 Switching (PSEL5) */
#define _00_PORT_PSEL3_PB5      (0x00U) /* PB5 general I/O port function is selected */
#define _20_PORT_PSEL3_PC3      (0x20U) /* PC3 general I/O port function is selected */

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define _0F_PDR1_DEFAULT        (0x0FU) /* PDR1 default value */
#define _3F_PDR2_DEFAULT        (0x3FU) /* PDR2 default value */
#define _DF_PDR3_DEFAULT        (0xDFU) /* PDR3 default value */
#define _B8_PDR4_DEFAULT        (0xB8U) /* PDR4 default value */
#define _A5_PDRA_DEFAULT        (0xA5U) /* PDRA default value */
#define _D4_PDRB_DEFAULT        (0xD4U) /* PDRB default value */
#define _60_PDRE_DEFAULT        (0x60U) /* PDRE default value */
#define _3F_PDRJ_DEFAULT        (0x3FU) /* PDRJ default value */


void R_PORT_Create(void);


#endif
