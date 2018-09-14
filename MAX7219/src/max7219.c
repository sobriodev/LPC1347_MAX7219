/*
 * max7219.c
 *
 *  Created on: 8 Sep 2018
 *      Author: sobriodev
 */

#include "max7219.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

STATIC MAX7219Config config;

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/**
 * @brief Wait until transmit FIFO is not full
 * @return Nothing
 */
STATIC INLINE void TFEWait(void) {
    while (!(config.ssp->SR & (1 << 1)));
}

/**
 * @brief Wait until the SPI controller is idle
 * @return Nothing
 */
STATIC INLINE void BSYWait(void) {
    while (config.ssp->SR & (1 << 4));
}

/**
 * @brief Send SPI frame
 * @param frame : Frame to send
 * @return Nothing
 */
STATIC void SPISendFrame(uint16_t frame) {
    TFEWait();
    config.ssp->DR = frame;
}

STATIC void sendFrame(uint8_t realFrames, uint8_t noOpFrames, uint16_t frame) {
    BSYWait();
    *(config.ssel) = LOW;
    for (int8_t i = 0; i < realFrames; ++i) {
        SPISendFrame(frame);
    }
    for (int8_t i = 0; i < noOpFrames; ++i) {
        SPISendFrame(MAX7219_FRAME(NO_OP_REG, 0x00));
    }
    BSYWait();
    *(config.ssel) = HIGH;
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

void MAX7219Configure(MAX7219Config cfg) {
    config = cfg;
    *(config.ssel) = HIGH;
    sendToAll(MAX7219_FRAME(DECODE_MODE_REG, 0x00));
    sendToAll(MAX7219_FRAME(SCAN_LIMIT_REG, 0x07));
    sendToAll(MAX7219_FRAME(DISPLAY_TEST_REG, 0x00));
}

void sendToAll(uint16_t frame) {
    sendFrame(config.numOfMatrices, 0, frame);
}

void sendToOne(uint8_t offset, uint16_t frame) {
    sendFrame(1, offset, frame);
}

void setColumnLEDs(uint8_t col, uint8_t data) {
    sendToOne(MATRIX_OFFSET(col), MAX7219_FRAME(DIGIT_REG(REAL_COLUMN(col)), data));
}
