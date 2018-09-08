/*
 * max7219.h
 *
 *  Created on: 8 Sep 2018
 *      Author: sobriodev
 */

#ifndef MAX7219_H_
#define MAX7219_H_

#include "board.h"

/* ------ Macros ------ */

#define NO_OP_REG                   0x00
#define DECODE_MODE_REG             0x09
#define INTENSITY_REG               0x0A
#define SCAN_LIMIT_REG              0x0B
#define SHOUTDOWN_REG               0x0C
#define DISPLAY_TEST_REG            0x0F
#define DIGIT_REG(DIGIT)            ((DIGIT) + 1)

#define MAX7219_FRAME(ADDR, DATA)   ((uint16_t) (((ADDR) << 8) | (DATA)))

/* ---- End Macros ---- */

/* ------ Private variables and functions ------ */
/* ---- End private variables end functions ---- */

/* ------ Public variables and functions ------ */
/* ---- End public variables end functions ---- */

#endif /* MAX7219_H_ */
