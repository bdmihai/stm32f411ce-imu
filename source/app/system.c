/*_____________________________________________________________________________
 │                                                                            |
 │ COPYRIGHT (C) 2021 Mihai Baneu                                             |
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
 |  Author: Mihai Baneu                           Last modified: 24.Jan.2021  |
 |                                                                            |
 |___________________________________________________________________________*/

#include "stm32f4xx.h"
#include "stm32rtos.h"
#include "gpio.h"
#include "system.h"

void system_init()
{
    /* configure Flash prefetch, Instruction cache, Data cache */ 
    SET_BIT(FLASH->ACR, FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN);

    /* configure the main internal regulator output voltage */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWREN);
    MODIFY_REG(PWR->CR, PWR_CR_VOS_Msk, PWR_CR_VOS_1);

    /* enable the external High Speed Clock @25MHz*/
    SET_BIT(RCC->CR, RCC_CR_HSEON);
    do {
    } while ((RCC->CR & RCC_CR_HSERDY_Msk) != RCC_CR_HSERDY);

    /* configure the PLL */
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_Msk, RCC_PLLCFGR_PLLSRC_HSE);
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLM_Msk, 25 << RCC_PLLCFGR_PLLM_Pos);
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLN_Msk, 192 << RCC_PLLCFGR_PLLN_Pos);
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLP_Msk, 0);
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLQ_Msk, 4 << RCC_PLLCFGR_PLLQ_Pos);

    /* enable PLL clock generation */
    SET_BIT(RCC->CR, RCC_CR_PLLON);
    do {
    } while ((RCC->CR & RCC_CR_PLLRDY_Msk) != RCC_CR_PLLRDY);

    /* to correctly read data from FLASH memory, the number of wait states (LATENCY) 
       must be correctly programmed according to the frequency of the CPU clock 
       (HCLK) of the device. */
    MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY_Msk, FLASH_ACR_LATENCY_3WS);

    /* set the highest APBx dividers in order to ensure that we do not go through
        a non-spec phase whatever we decrease or increase HCLK. */
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1_Msk, RCC_CFGR_PPRE1_DIV16);
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2_Msk, RCC_CFGR_PPRE2_DIV16);
    MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE_Msk, RCC_CFGR_HPRE_DIV1);

    /* set the SYSCLK to PLL */
    MODIFY_REG(RCC->CFGR, RCC_CFGR_SW_Msk, RCC_CFGR_SW_PLL);
    do {
    } while ((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_PLL);

    /* disable HSI clock */
    CLEAR_BIT(RCC->CR, RCC_CR_HSION);
    do {
    } while ((RCC->CR & RCC_CR_HSIRDY_Msk) == RCC_CR_HSIRDY);

    /* confgure the APB clocks */
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1_Msk, RCC_CFGR_PPRE1_DIV2);
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2_Msk, RCC_CFGR_PPRE2_DIV2);

    /* enable AHB1 ports clock */
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN);
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN);
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOHEN);

    /* enable APB1 devices */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C1EN);

    /* enable APB2 devices */
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM10EN);
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI1EN);

    /* one us timer for delay */
    TIM10->PSC = (configCPU_CLOCK_HZ / 1000000) - 1;
    MODIFY_REG(TIM10->CR1, TIM_CR1_CEN_Msk, TIM_CR1_CEN);

    /* stop timer when debuggng */
    SET_BIT(DBGMCU->APB2FZ, DBGMCU_APB2_FZ_DBG_TIM10_STOP);
}

/**
 * delay in us (blockig)
 */
void delay_us(const uint32_t us)
{
    SET_BIT(TIM10->EGR, TIM_EGR_UG);
    do {
    } while (TIM10->CNT < us );
}

/**
 * blink the led in case of problems
 * This function can only be called after the initialization phase. It is used for
 * faults after RTOS start.
*/
void blink(const uint8_t n)
{
    /* enable DWT */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    for (;;) {
        for (int i = 0; i < n; i++) {
            gpio_set_blue_led();
            DWT->CYCCNT = 0;
            do {
            } while (DWT->CYCCNT < (100 * (configCPU_CLOCK_HZ / 1000)));

            gpio_reset_blue_led();
            DWT->CYCCNT = 0;
            do {
            } while (DWT->CYCCNT < (400 * (configCPU_CLOCK_HZ / 1000)));
        }
        do {
        } while (DWT->CYCCNT < (2000 * (configCPU_CLOCK_HZ / 1000)));
    }
}

int _write(int file, char *ptr, int len)
{
    (void)(file);
    for (int i = 0; i < len; i++) {
        ITM_SendChar((*ptr++));
    }
    return len;
}

/** Hard fault - blink four short flash every two seconds */
void HardFault_Handler()
{
    blink(4);
}

/** Bus fault - blink five short flashes every two seconds */
void BusFault_Handler()
{
    blink(5);
}

/** Usage fault - blink six short flashes every two seconds */
void UsageFault_Handler()
{
    blink(6);
}
