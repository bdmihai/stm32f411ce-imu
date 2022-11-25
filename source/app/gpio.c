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
#include "stm32rtos.h"
#include "queue.h"
#include "gpio.h"
#include "rencoder.h"

void gpio_init()
{
    /* disable JTAG */
    MODIFY_REG(GPIOA->MODER,    GPIO_MODER_MODER15_Msk,     0);                         /* JTDI disabled */
    MODIFY_REG(GPIOB->MODER,    GPIO_MODER_MODER4_Msk,      0);                         /* NJTRST disabled */
    MODIFY_REG(GPIOB->MODER,    GPIO_MODER_MODER3_Msk,      0);                         /* JTDO disabled */

    /* configure LED pin */
    MODIFY_REG(GPIOC->MODER,    GPIO_MODER_MODER13_Msk,     GPIO_MODER_MODER13_0);      /* set the pin as output */
    MODIFY_REG(GPIOC->OTYPER,   GPIO_OTYPER_OT13_Msk,       0);                         /* push pull */
    MODIFY_REG(GPIOC->OSPEEDR,  GPIO_OSPEEDR_OSPEED13_Msk,  0);                         /* low speed */
    MODIFY_REG(GPIOC->PUPDR,    GPIO_PUPDR_PUPD13_Msk,      0);                         /* no pull up, no pull down */

    /* configuration of the I2C GPIO pins */
    MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODER6_Msk, GPIO_MODER_MODER6_1);                                   /* set the pin as alternate function */
    MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODER7_Msk, GPIO_MODER_MODER7_1);                                   /* set the pin as alternate function */

    MODIFY_REG(GPIOB->AFR[0], GPIO_AFRL_AFSEL6_Msk, GPIO_AFRL_AFSEL6_2);                                    /* AF4 - I2C1_SCL */
    MODIFY_REG(GPIOB->AFR[0], GPIO_AFRL_AFSEL7_Msk, GPIO_AFRL_AFSEL7_2);                                    /* AF4 - I2C1_SDA */

    MODIFY_REG(GPIOB->OTYPER, GPIO_OTYPER_OT6_Msk, GPIO_OTYPER_OT6);                                        /* open drain */
    MODIFY_REG(GPIOB->OTYPER, GPIO_OTYPER_OT7_Msk, GPIO_OTYPER_OT7);                                        /* open drain */

    MODIFY_REG(GPIOB->OSPEEDR, GPIO_OSPEEDR_OSPEED6_Msk, 0 );                                               /* low speed */
    MODIFY_REG(GPIOB->OSPEEDR, GPIO_OSPEEDR_OSPEED7_Msk, 0 );                                               /* low speed */

    MODIFY_REG(GPIOB->PUPDR, GPIO_PUPDR_PUPD6_Msk, 0);                                                      /* no pull up, no pull down */
    MODIFY_REG(GPIOB->PUPDR, GPIO_PUPDR_PUPD7_Msk, 0);                                                      /* no pull up, no pull down */

    /* configuration of the rotary encoder GPIO pins */
    MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODER0_Msk,  0);                              /* set the pin as input */
    MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODER1_Msk,  0);                              /* set the pin as input */
    MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODER10_Msk, 0);                              /* set the pin as input */

    MODIFY_REG(GPIOB->PUPDR, GPIO_PUPDR_PUPD0_Msk,  0);                               /* no pull up, no pull down */
    MODIFY_REG(GPIOB->PUPDR, GPIO_PUPDR_PUPD1_Msk,  0);                               /* no pull up, no pull down */
    MODIFY_REG(GPIOB->PUPDR, GPIO_PUPDR_PUPD10_Msk, 0);                               /* no pull up, no pull down */

    MODIFY_REG(SYSCFG->EXTICR[0], SYSCFG_EXTICR1_EXTI0,  SYSCFG_EXTICR1_EXTI0_PB);    /* map gpio to EXTI lines */
    MODIFY_REG(SYSCFG->EXTICR[0], SYSCFG_EXTICR1_EXTI1,  SYSCFG_EXTICR1_EXTI1_PB);    /* map gpio to EXTI lines */
    MODIFY_REG(SYSCFG->EXTICR[2], SYSCFG_EXTICR3_EXTI10, SYSCFG_EXTICR3_EXTI10_PB);   /* map gpio to EXTI lines */

    /* configure tft control pins */
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER6_Msk, GPIO_MODER_MODER6_0);   /* set the pin as output */
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER3_Msk, GPIO_MODER_MODER3_0);   /* set the pin as output */

    MODIFY_REG(GPIOA->OTYPER, GPIO_OTYPER_OT6_Msk, 0);                      /* push pull */
    MODIFY_REG(GPIOA->OTYPER, GPIO_OTYPER_OT3_Msk, 0);                      /* push pull */

    MODIFY_REG(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED6_Msk, 0);                /* low speed */
    MODIFY_REG(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED3_Msk, 0);                /* low speed */

    MODIFY_REG(GPIOA->PUPDR, GPIO_PUPDR_PUPD6_Msk, 0);                      /* no pull up, no pull down */
    MODIFY_REG(GPIOA->PUPDR, GPIO_PUPDR_PUPD3_Msk, 0);                      /* no pull up, no pull down */

    gpio_tft_res_high();
    gpio_tft_dc_low();

    /* configure the SPI pins */
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER4_Msk, GPIO_MODER_MODER4_1);                                     /* set the pin as alternate function */
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER5_Msk, GPIO_MODER_MODER5_1);                                     /* set the pin as alternate function */
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER7_Msk, GPIO_MODER_MODER7_1);                                     /* set the pin as alternate function */

    MODIFY_REG(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED4_Msk, GPIO_OSPEEDR_OSPEED4_0 | GPIO_OSPEEDR_OSPEED4_1 );   /* high speed */
    MODIFY_REG(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED5_Msk, GPIO_OSPEEDR_OSPEED5_0 | GPIO_OSPEEDR_OSPEED5_1 );   /* high speed */
    MODIFY_REG(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED7_Msk, GPIO_OSPEEDR_OSPEED7_0 | GPIO_OSPEEDR_OSPEED7_1 );   /* high speed */

    MODIFY_REG(GPIOA->AFR[0], GPIO_AFRL_AFSEL4_Msk, 5 << GPIO_AFRL_AFSEL4_Pos);                               /* AF05 - SPI1_NSS */
    MODIFY_REG(GPIOA->AFR[0], GPIO_AFRL_AFSEL5_Msk, 5 << GPIO_AFRL_AFSEL5_Pos);                               /* AF05 - SPI1_SCK */
    MODIFY_REG(GPIOA->AFR[0], GPIO_AFRL_AFSEL7_Msk, 5 << GPIO_AFRL_AFSEL7_Pos);                               /* AF05 - SPI1_MOSI */

    MODIFY_REG(GPIOA->PUPDR, GPIO_PUPDR_PUPD4_Msk, 1 << GPIO_PUPDR_PUPD4_Pos);                                /* pull up */
}

void gpio_set_blue_led()
{
    GPIOC->BSRR = GPIO_BSRR_BR13;
}

void gpio_reset_blue_led()
{
    GPIOC->BSRR = GPIO_BSRR_BS13;
}

void gpio_toggle_blue_led()
{
    GPIOC->BSRR = (GPIOB->IDR & GPIO_IDR_IDR_13) ? GPIO_BSRR_BR13 : GPIO_BSRR_BS13;
}

void gpio_handle_rotation()
{
  rencoder_input_event_t event;
  event.type = rencoder_input_rotation;
  event.gpio = GPIOB->IDR & 0x03;
  xQueueSendToBackFromISR(rencoder_input_queue, &event, (TickType_t) 0);
}

void gpio_handle_key()
{
  rencoder_input_event_t event;
  event.type = rencoder_input_key;
  event.gpio = (GPIOB->IDR & GPIO_IDR_ID10_Msk) ? 1 : 0;
  xQueueSendToBackFromISR(rencoder_input_queue, &event, (TickType_t) 0);
}

void gpio_tft_dc_high()
{
  GPIOA->BSRR = GPIO_BSRR_BS6;
}

void gpio_tft_dc_low()
{
  GPIOA->BSRR = GPIO_BSRR_BR6;
}

void gpio_tft_res_high()
{
  GPIOA->BSRR = GPIO_BSRR_BS3;
}

void gpio_tft_res_low()
{
  GPIOA->BSRR = GPIO_BSRR_BR3;
}
