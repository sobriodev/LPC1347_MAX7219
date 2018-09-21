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
#define REAL_COLUMN(COL)            ((COL) % 8)

#define HIGH                        1
#define LOW                         0

typedef struct {
    LPC_SSP_T *ssp;         /* Pointer to used SSP. MUST be pre-configured */
    __IO uint8_t *ssel;     /* SSEL pin used for latching data in MAX7219's. MUST be pre-configured as GPIO output pin */
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
 * @param offset : Matrix offset
 * @param frame  : Frame to send
 * @return Nothing
 */
void sendToOne(uint8_t offset, uint16_t frame);

/**
 * @brief Clear (toggle all LEDs off) desired matrix
 * @param offset : Matrix offset
 */
void clearMatrix(uint8_t offset);

/**
 * @brief Set desired column LEDs
 * @param col  : Column number (numbered from zero)
 * @param data : LED states
 */
STATIC INLINE void setColumnLEDs(uint8_t col, uint8_t data) {
    sendToOne(MATRIX_OFFSET(col), MAX7219_FRAME(DIGIT_REG(REAL_COLUMN(col)), data));
}

/**
 * @brief Set desired matrix shutdown mode
 * @param offset : Matrix offset
 * @param mode   : True = shutdown enabled, false = shutdown disabled
 */
STATIC INLINE void setShutdownMode(uint8_t offset, bool mode) {
    sendToOne(offset, MAX7219_FRAME(SHOUTDOWN_REG, !mode));
}

/**
 * @brief Set desired matrix test mode
 * @param offset : Matrix offset
 * @param mode   : True = test mode enabled, false = test mode disabled
 */
STATIC INLINE void setTestMode(uint8_t offset, bool mode) {
    sendToOne(offset, MAX7219_FRAME(DISPLAY_TEST_REG, mode));
}

/**
 * @brief Set desired matrix intensity
 * @param offset    : Matrix offset
 * @param intensity : Intensity from 0x00 (minimum) to 0x0F (maximum)
 */
STATIC INLINE void setIntensity(uint8_t offset, uint8_t intensity) {
    sendToOne(offset, MAX7219_FRAME(INTENSITY_REG, intensity));
}

#endif /* MAX7219_H_ */
