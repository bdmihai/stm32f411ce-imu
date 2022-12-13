/*_____________________________________________________________________________
 │                                                                            |
 │ COPYRIGHT (C) 2022 Mihai Baneu                                             |
 │                                                                            |
 | Permission is hereby  granted,  free of charge,  to any person obtaining a |
 | copy of this software and associated documentation files (the "Software"), |
 | to deal in the Software without restriction,  including without limitation |
 | the rights to  use, copy, modify, merge, publish, distribute,  sublicense, |
 | and/or sell copies  of  the Software, and to permit  persons to  whom  the |
 | Software is furnished to do so, subject to the following conditions:       |
 |                                                                            |
 | The above  copyright notice  and this permission notice  shall be included |
 | in all copies or substantial portions of the Software.                     |
 |                                                                            |
 | THE SOFTWARE IS PROVIDED  "AS IS",  WITHOUT WARRANTY OF ANY KIND,  EXPRESS |
 | OR   IMPLIED,   INCLUDING   BUT   NOT   LIMITED   TO   THE  WARRANTIES  OF |
 | MERCHANTABILITY,  FITNESS FOR  A  PARTICULAR  PURPOSE AND NONINFRINGEMENT. |
 | IN NO  EVENT SHALL  THE AUTHORS  OR  COPYRIGHT  HOLDERS  BE LIABLE FOR ANY |
 | CLAIM, DAMAGES OR OTHER LIABILITY,  WHETHER IN AN ACTION OF CONTRACT, TORT |
 | OR OTHERWISE, ARISING FROM,  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR  |
 | THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                 |
 |____________________________________________________________________________|
 |                                                                            |
 |  Author: Mihai Baneu                           Last modified: 05.Oct.2022  |
 |                                                                            |
 |___________________________________________________________________________*/

#include "stm32f4xx.h"
#include "spi.h"

void spi_init()
{
    MODIFY_REG(SPI1->CR1, SPI_CR1_CPHA_Msk, 0);                       /* CPOL = 0, CPHA = 0 */
    MODIFY_REG(SPI1->CR1, SPI_CR1_CPOL_Msk, 0);
    MODIFY_REG(SPI1->CR1, SPI_CR1_MSTR_Msk, SPI_CR1_MSTR);            /* MASTER */
    MODIFY_REG(SPI1->CR1, SPI_CR1_BR_Msk, 0x02 << SPI_CR1_BR_Pos);    /* 48Mhz / 2 = 24Mhz (41.67ns) */
    MODIFY_REG(SPI1->CR1, SPI_CR1_LSBFIRST_Msk, 0);                   /* MSB First */
    MODIFY_REG(SPI1->CR1, SPI_CR1_BIDIMODE_Msk, SPI_CR1_BIDIMODE);    /* BI Directional - MOSI */
    MODIFY_REG(SPI1->CR1, SPI_CR1_BIDIOE_Msk, SPI_CR1_BIDIOE);        /* BI Directional - transmit only */

    MODIFY_REG(SPI1->CR2, SPI_CR2_SSOE_Msk, SPI_CR2_SSOE);            /* single master */
}

uint16_t spi_write(const uint8_t *buffer, uint16_t size, uint16_t repeat)
{
    /* set the SPI in transmit only mode */
    MODIFY_REG(SPI1->CR1, SPI_CR1_BIDIOE_Msk, SPI_CR1_BIDIOE);

    /* activate the SPI */
    MODIFY_REG(SPI1->CR1, SPI_CR1_SPE_Msk, SPI_CR1_SPE);

    for (uint16_t j = 0; j < repeat; j++) {
        for (uint16_t i = 0; i < size; i++) {
            /* wait for TX ready and load the data */
            do {
            } while ((SPI1->SR & SPI_SR_TXE_Msk) != SPI_SR_TXE);
            SPI1->DR = buffer[i];
        }
    }

    /* wait for TX ready, wait for busy flag to be reseted and then disable the SPI */
    do {
    } while ((SPI1->SR & SPI_SR_TXE_Msk) != SPI_SR_TXE);
    do {
    } while ((SPI1->SR & SPI_SR_BSY_Msk) == SPI_SR_BSY);
    MODIFY_REG(SPI1->CR1, SPI_CR1_SPE_Msk, 0);

    return size;
}

uint16_t spi_read(uint8_t *buffer, uint16_t size)
{
    /* set the SPI in receive only mode */
    MODIFY_REG(SPI1->CR1, SPI_CR1_BIDIOE_Msk, 0);

    /* activate the SPI */
    MODIFY_REG(SPI1->CR1, SPI_CR1_SPE_Msk, SPI_CR1_SPE);

    for (uint16_t i = 0; i < size; i++) {
        /* disable the SPI */
        if (i == (size - 1)) {
            MODIFY_REG(SPI1->CR1, SPI_CR1_SPE_Msk, 0);
        }

        /* wait for data */
        do {
        } while ((SPI1->SR & SPI_SR_RXNE_Msk) != SPI_SR_RXNE);

        buffer[i] = SPI1->DR;
    }

    return size;
}