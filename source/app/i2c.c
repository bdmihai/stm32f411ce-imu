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
 |  Author: Mihai Baneu                           Last modified: 23.Jul.2022  |
 |                                                                            |
 |___________________________________________________________________________*/

#include "stm32f4xx.h"
#include "i2c.h"

void i2c_init()
{
    MODIFY_REG(I2C1->CR2, I2C_CR2_FREQ_Msk, 48 << I2C_CR2_FREQ_Pos);            /* match the APB2 frequency */

    MODIFY_REG(I2C1->CCR, I2C_CCR_FS_Msk, I2C_CCR_FS);                          /* fast mode @400kHz                                       */
    MODIFY_REG(I2C1->CCR, I2C_CCR_DUTY_Msk, 0);                                 /* duty tlow/thigh = 2                                     */
    MODIFY_REG(I2C1->CCR, I2C_CCR_CCR_Msk, 40 << I2C_CCR_CCR_Pos);              /* thigh+tlow = 3 * CCR * tpclk => CCR = pclk / (3*400kHz) */

    MODIFY_REG(I2C1->TRISE, I2C_TRISE_TRISE_Msk, 15 << I2C_TRISE_TRISE_Pos);    /* trise = pclk[MHz] * 300 / 1000 + 1 */

    MODIFY_REG(I2C1->FLTR, I2C_FLTR_ANOFF_Msk, 0 << I2C_FLTR_ANOFF_Pos);        /* keep analog filter on   */
    MODIFY_REG(I2C1->FLTR, I2C_FLTR_DNF_Msk, 0 << I2C_FLTR_DNF_Pos);            /* keep digital filter off */

    MODIFY_REG(I2C1->CR1, I2C_CR1_PE_Msk, I2C_CR1_PE);                          /* enable i2c */
}

uint16_t i2c_write(uint8_t address, const uint8_t *buffer, uint16_t size)
{
    // generate a start condition and wait for it to be sent
    MODIFY_REG(I2C1->CR1, I2C_CR1_START_Msk, I2C_CR1_START);
    do {
    } while ((I2C1->SR1 & I2C_SR1_SB_Msk) != I2C_SR1_SB);

    // send address and wait for ADDR and TRA
    I2C1->DR = (address << 1);
    do {
    } while ((I2C1->SR1 & I2C_SR1_ADDR_Msk) != I2C_SR1_ADDR);
    do {
    } while ((I2C1->SR2 & I2C_SR2_MSL_Msk) != I2C_SR2_MSL);

    // wait for TX ready
    do {
    } while ((I2C1->SR1 & I2C_SR1_TXE_Msk) != I2C_SR1_TXE);

    for (uint16_t i = 0; i < size; i++) {
        // write the data to the data register
        I2C1->DR = buffer[i];

        // wait for TX ready
        do {
        } while ((I2C1->SR1 & I2C_SR1_TXE_Msk) != I2C_SR1_TXE);
    }

    // wait for byte transfer finished
    do {
    } while ((I2C1->SR1 & I2C_SR1_BTF_Msk) != I2C_SR1_BTF);

    // generate a stop condition
    MODIFY_REG(I2C1->CR1, I2C_CR1_STOP_Msk, I2C_CR1_STOP);

    // wait to go back to slave mode
    do {
    } while ((I2C1->SR2 & I2C_SR2_MSL_Msk) == I2C_SR2_MSL);

    return size;
}

uint16_t i2c_read(uint8_t address, uint8_t *buffer, uint16_t size)
{
    // generate a start condition and wait for it to be sent
    MODIFY_REG(I2C1->CR1, I2C_CR1_START_Msk, I2C_CR1_START);
    do {
    } while ((I2C1->SR1 & I2C_SR1_SB_Msk) != I2C_SR1_SB);

    // send address and wait for ADDR and TRA
    I2C1->DR = (address << 1) | 0x01;
    do {
    } while ((I2C1->SR1 & I2C_SR1_ADDR_Msk) != I2C_SR1_ADDR);
    // clear the ACK if only one byte is to be received
    MODIFY_REG(I2C1->CR1, I2C_CR1_ACK_Msk, ((size > 1) ? I2C_CR1_ACK : 0));
    do {
    } while ((I2C1->SR2 & I2C_SR2_MSL_Msk) != I2C_SR2_MSL);

    for (uint16_t i = 0; i < size - 1; i++)
    {
        // wait for RX ready
        do {
        } while ((I2C1->SR1 & I2C_SR1_RXNE_Msk) != I2C_SR1_RXNE);

        // read the data from the register
        buffer[i] = I2C1->DR;
    }

    MODIFY_REG(I2C1->CR1, I2C_CR1_ACK_Msk, 0);
    MODIFY_REG(I2C1->CR1, I2C_CR1_STOP_Msk, I2C_CR1_STOP);
    
    // wait for RX ready
    do {
    } while ((I2C1->SR1 & I2C_SR1_RXNE_Msk) != I2C_SR1_RXNE);

    // read the data from the register
    buffer[size - 1] = I2C1->DR;

    return size;
}
