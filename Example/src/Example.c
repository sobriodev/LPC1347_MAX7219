/*
 * Example.c
 *
 *  Created on: 8 Sep 2018
 *      Author: sobriodev
 */

#include "board.h"
#include "max7219.h"
#include <cr_section_macros.h>

#define SCK_PORT    1
#define SCK_PIN     15
#define MOSI_PORT   0
#define MOSI_PIN    21
#define MISO_PORT   0
#define MISO_PIN    22
#define SSEL_PORT   1
#define SSEL_PIN    19

int main(void) {
    /* Generic initialization */
    SystemCoreClockUpdate();
    Board_Init();

    LPC_IOCON->PIO1[SCK_PIN] = 0x3;
    LPC_IOCON->PIO0[MOSI_PIN] = 0x2;
    LPC_IOCON->PIO0[MISO_PIN] = 0x3;
    LPC_IOCON->PIO1[SSEL_PIN] = 0x0;
    LPC_GPIO_PORT->DIR[SSEL_PORT] |= 1 << SSEL_PIN;
    LPC_GPIO_PORT->B[SSEL_PORT][SSEL_PIN] = 1;

    LPC_SYSCTL->SYSAHBCLKCTRL |= 1 << 18;

    LPC_SYSCTL->SSP1CLKDIV = 1;

    LPC_SYSCTL->PRESETCTRL |= 1 << 2;

    LPC_SSP1->CR0 = (LPC_SSP1->CR0 & ~0xF) | 0xF;
    LPC_SSP1->CR0 &= ~(0x3 << 4);
    LPC_SSP1->CR0 &= ~(1 << 6 | 1 << 7);
    LPC_SSP1->CR0 &= ~(0xF << 8);

    LPC_SSP1->CR1 &= ~(1 << 2);
    LPC_SSP1->CPSR = 254;
    LPC_SSP1->CR1 |= 1 << 1;

    MAX7219Config config = {LPC_SSP1, &(LPC_GPIO_PORT->B[1][19]), 2};
    MAX7219Configure(config);

    clearMatrix(0);
    clearMatrix(1);

    setShutdownMode(0, false);
    setShutdownMode(1, false);

    const uint64_t data[] = {DIGIT_1, DIGIT_2, DIGIT_3};

    int i = 0;
    if (setAnimation((uint8_t *) data, 24)) {
        LPC_GPIO_PORT->B[0][7] = true;
        while (nextFrame()) {
            i = 0;
            while (++i < 300000);
        }
    }

    i = 0;
    while (++i < 1000000);

    clearMatrix(0);
    clearMatrix(1);

    i = 0;
    while (++i < 1000000);

    const uint64_t data2[] = {DIGIT_4, DIGIT_3, DIGIT_2, DIGIT_1};

    i = 0;
    if (setAnimation((uint8_t *) data2, 32)) {
        LPC_GPIO_PORT->B[0][7] = true;
        while (nextFrame()) {
            i = 0;
            while (++i < 300000);
        }
    }

    MAX7219Destroy();

    while(1);
    return 0;
}
