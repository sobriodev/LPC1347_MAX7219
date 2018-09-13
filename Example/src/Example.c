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

    MAX7219Config config = {LPC_SSP1, &(LPC_GPIO_PORT->B[1][19]), 8};
    MAX7219Configure(config);
    sendToAll(10);

    while(1);
    return 0;
}
