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

static MAX7219Config config;

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

void MAX7219Configure(MAX7219Config cfg) {
    config = cfg;
    if (config.numOfMatrices > 8) { config.numOfMatrices = 8; }
    *(config.ssel) = 1;
}

void sendToAll(uint16_t frame) {
    while ((config.ssp->SR & (1 << 4))); /* Wait until the SPI controller is idle */
    *(config.ssel) = 0;
    for (int8_t i = 0; i < config.numOfMatrices; ++i) {
        config.ssp->DR = frame;
    }
    while ((config.ssp->SR & (1 << 4))); /* Wait until the SPI controller is idle */
    *(config.ssel) = 1;
}

void sendToOne(uint8_t offset, uint16_t frame) {}

void sendConfigurationFrames(void) {}
