/*
 * max7219.c
 *
 *  Created on: 8 Sep 2018
 *      Author: sobriodev
 */

#include "max7219.h"
#include "stdlib.h"
#include "string.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

typedef struct {
    size_t length;              /* Animation length in bytes (number of columns) */
    const uint8_t *start;       /* Pointer to the first animation byte (first column LEDs) */
    const uint8_t *stop;        /* Pointer to the last animation byte (last column LEDs) */
    const uint8_t *currentCol;  /* Pointer to the current animation byte (current column LEDs) */
    bool firstPass;             /* Animation pass status. True means that it is first pass */
} MAX7219Animation;


STATIC MAX7219Config config;  /* Configuration settings */
STATIC MAX7219Animation anim; /* Animation settings */
STATIC bool animReady;        /* Animation ready status. True means that animation is ready to play */
STATIC uint64_t *buffer;      /* Buffer for storing current animation bytes */

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
    anim.currentCol = anim.start = anim.stop = NULL;
    buffer = NULL;
    *(config.ssel) = HIGH;
    sendToAll(MAX7219_FRAME(DECODE_MODE_REG, 0x00)); /* Disable code B */
    sendToAll(MAX7219_FRAME(SCAN_LIMIT_REG, 0x07)); /* Set scan limit to 8 columns */
    sendToAll(MAX7219_FRAME(DISPLAY_TEST_REG, 0x00)); /* Disable test mode */
}

void sendToAll(uint16_t frame) {
    BSYWait();
    *(config.ssel) = LOW;
    for (int8_t i = 0; i < config.numOfMatrices; ++i) {
        SPISendFrame(frame);
    }
    BSYWait();
    *(config.ssel) = HIGH;

}

void sendToOne(uint8_t offset, uint16_t frame) {
    BSYWait();
    *(config.ssel) = LOW;
    for (int8_t i = config.numOfMatrices - 1; i >= 0; --i) {
        if (i == offset) {
            SPISendFrame(frame);
        }
        else {
            SPISendFrame(MAX7219_FRAME(NO_OP_REG, 0x00));
        }
    }
    BSYWait();
    *(config.ssel) = HIGH;
}

void clearMatrix(uint8_t offset) {
    for (int8_t i = 0; i < 8; ++i) {
        sendToOne(offset, MAX7219_FRAME(DIGIT_REG(i), 0x00));
    }
}

void setPattern(uint8_t offset, uint64_t pattern) {
    for (int8_t i = 0; i < 8; ++i) {
        uint8_t column = (uint8_t) i + (offset * 8); /* Calculate column offset */
        uint8_t columnLEDs = (uint8_t) (pattern >> (i * 8)) & 0xFF; /* Calculate column LEDs from 64 bit value */
        setColumnLEDs(column, columnLEDs);
    }
}

void setPatterns(const uint64_t *patterns) {
    for (int8_t i = 0; i < 8; ++i) {
        BSYWait();
        *(config.ssel) = LOW;
        for (int8_t j = config.numOfMatrices - 1; j >= 0; --j) {
            uint8_t frameData = (patterns[j] >> (i * 8)) & 0xFF; /* Extract the right column byte from the data */
            SPISendFrame(MAX7219_FRAME(DIGIT_REG(i), frameData));
        }
        BSYWait();
        *(config.ssel) = HIGH;
    }
}

bool setAnimation(const uint8_t *data, size_t animationLength) {
    if (animationLength == 0 || animationLength <= config.numOfMatrices * 8) { /* If animation is shorter than matrices column do nothing */
        animReady = false;
    } else {
        buffer = (uint64_t *) malloc(config.numOfMatrices * sizeof(uint64_t)); /* Allocate needed memory */
        if (buffer == NULL) {
            animReady = false; /* Memory cannot be allocated. Do nothing */
        }
        anim.length = animationLength;
        anim.start = anim.currentCol = data;
        anim.stop = data + animationLength - 1;
        anim.firstPass = true;
        animReady = true;
    }
    return animReady;
}

bool nextFrame(void) {
    if (!animReady) { return true; } /* If animation is not created correctly just return true */
    size_t tailLength = anim.stop - anim.currentCol + 1; /* Calculate the number of bytes to the right of actual position */
    if (tailLength >= config.numOfMatrices * 8) { /* If the number of bytes to the right is greater or equal number of columns just copy them to the buffer */
        memcpy(buffer, anim.currentCol, config.numOfMatrices * 8);
    } else { /* Otherwise copy tail bytes and calculated head bytes */
        memcpy(buffer, anim.currentCol, tailLength);
        memcpy((uint8_t *) buffer + tailLength, anim.start, config.numOfMatrices * 8 - tailLength);
    }
    setPatterns(buffer); /* Update the LEDs */
    ++anim.currentCol; /* Increment actual column number */
    if (anim.currentCol > anim.stop) { anim.currentCol = anim.start; } /* Back to the start column if it is out of range */
    if (anim.currentCol == anim.start + 1) { /* If second column is reached return suitable animation status according to the pass status */
        if (anim.firstPass) {
            anim.firstPass = false;
            return true;
        }
        return false;
    }
    return true;
}

void MAX7219Destroy(void) {
    free(buffer);
    buffer = NULL;
}
