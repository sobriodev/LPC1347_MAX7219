/*
 * max7219.h
 *
 *  Created on: 8 Sep 2018
 *      Author: sobriodev
 */

#ifndef MAX7219_H_
#define MAX7219_H_

#include "board.h"

#define NO_OP_REG                   0x00
#define DECODE_MODE_REG             0x09
#define INTENSITY_REG               0x0A
#define SCAN_LIMIT_REG              0x0B
#define SHOUTDOWN_REG               0x0C
#define DISPLAY_TEST_REG            0x0F
#define DIGIT_REG(DIGIT)            ((DIGIT) + 1)
#define MAX7219_FRAME(ADDR, DATA)   ((uint16_t) (((ADDR) << 8) | (DATA)))

#define MATRIX_OFFSET(COL)          ((COL) / 8)

typedef struct {
    LPC_SSP_T *ssp;         /* Pointer to used SSP. MUST be pre-configured */
    uint8_t *ssel;          /* SSEL pin used for latching data in MAX7219's. MUST be pre-configured as GPIO output pin */
    uint8_t numOfMatrices;  /* Number of MAX7219 matrices used */
} MAX7219Config;

/**
 * @brief Initialize MAX7219 configuration
 * @param cfg : Configuration struct
 * @return Nothing
 */
void MAX7219Configure(MAX7219Config cfg);

/**
 * @brief Send frame to all matrices
 * @param frame : Frame to send
 * @return Nothing
 */
void sendToAll(uint16_t frame);

/**
 * @brief Send frame to particular matrix
 * @param frame : Frame to send
 * @return Nothing
 */
void sendToOne(uint8_t offset, uint16_t frame);

/**
 * @brief Send configuration frames to all matrices
 */
void sendConfigurationFrames(void);

#endif /* MAX7219_H_ */
