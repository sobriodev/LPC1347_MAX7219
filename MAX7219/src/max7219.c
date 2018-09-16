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
    BSYWait();
    *(config.ssel) = LOW;
    for (int8_t i = 0; i < config.numOfMatrices; ++i) { SPISendFrame(frame); }
    BSYWait();
    *(config.ssel) = HIGH;

}

void sendToOne(uint8_t offset, uint16_t frame) {
    BSYWait();
    *(config.ssel) = LOW;
    for (int8_t i = config.numOfMatrices - 1; i >= 0; --i) {
        if (i == offset) { SPISendFrame(frame); }
        else { SPISendFrame(MAX7219_FRAME(NO_OP_REG, 0x00)); }
    }
    BSYWait();
    *(config.ssel) = HIGH;
}

void setColumnLEDs(uint8_t col, uint8_t data) {
    sendToOne(MATRIX_OFFSET(col), MAX7219_FRAME(DIGIT_REG(REAL_COLUMN(col)), data));
}
