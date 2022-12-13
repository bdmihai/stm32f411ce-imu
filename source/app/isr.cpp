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
 |  Author: Mihai Baneu                           Last modified: 14.Nov.2021  |
 |                                                                            |
 |___________________________________________________________________________*/

#include "stm32f4xx.h"
#include "stm32rtos.h"
#include "queue.h"
#include "isr.h"
#include "system.h"
#include "gpio.h"
#include "dma.h"

void isr_init()
{
    /* mask the EXTI lines as interupts */
    MODIFY_REG(EXTI->IMR, EXTI_IMR_MR0_Msk,  EXTI_IMR_MR0);
    MODIFY_REG(EXTI->IMR, EXTI_IMR_MR1_Msk,  EXTI_IMR_MR1);
    MODIFY_REG(EXTI->IMR, EXTI_IMR_MR9_Msk,  EXTI_IMR_MR9);
    MODIFY_REG(EXTI->IMR, EXTI_IMR_MR10_Msk, EXTI_IMR_MR10);

    /* set rising/falling edge as trigger */
    MODIFY_REG(EXTI->RTSR, EXTI_RTSR_TR0_Msk,  EXTI_RTSR_TR0);
    MODIFY_REG(EXTI->FTSR, EXTI_FTSR_TR0_Msk,  EXTI_FTSR_TR0);
    MODIFY_REG(EXTI->RTSR, EXTI_RTSR_TR1_Msk,  EXTI_RTSR_TR1);
    MODIFY_REG(EXTI->FTSR, EXTI_FTSR_TR1_Msk,  EXTI_FTSR_TR1);
    MODIFY_REG(EXTI->RTSR, EXTI_RTSR_TR9_Msk,  EXTI_RTSR_TR9);
    MODIFY_REG(EXTI->FTSR, EXTI_FTSR_TR9_Msk,  0);
    MODIFY_REG(EXTI->RTSR, EXTI_RTSR_TR10_Msk, EXTI_RTSR_TR10);
    MODIFY_REG(EXTI->FTSR, EXTI_FTSR_TR10_Msk, EXTI_FTSR_TR10);

    /* enable interupt */
    NVIC_SetPriority(EXTI0_IRQn,        NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 11 /* PreemptPriority */, 0 /* SubPriority */));
    NVIC_SetPriority(EXTI1_IRQn,        NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 11 /* PreemptPriority */, 0 /* SubPriority */));
    NVIC_SetPriority(EXTI9_5_IRQn,      NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 11 /* PreemptPriority */, 0 /* SubPriority */));
    NVIC_SetPriority(EXTI15_10_IRQn,    NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 11 /* PreemptPriority */, 0 /* SubPriority */));
    NVIC_SetPriority(DMA1_Stream0_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 11 /* PreemptPriority */, 0 /* SubPriority */));
    NVIC_SetPriority(DMA1_Stream1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 11 /* PreemptPriority */, 0 /* SubPriority */));

    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_EnableIRQ(EXTI1_IRQn);
    NVIC_EnableIRQ(EXTI9_5_IRQn);
    NVIC_EnableIRQ(EXTI15_10_IRQn);
    NVIC_EnableIRQ(DMA1_Stream0_IRQn);
    NVIC_EnableIRQ(DMA1_Stream1_IRQn);
}

/** Hard fault - blink four short flash every two seconds */
extern "C" void HardFault_Handler()
{
    blink(4);
}

/** Bus fault - blink five short flashes every two seconds */
extern "C" void BusFault_Handler()
{
    blink(5);
}

/** Usage fault - blink six short flashes every two seconds */
extern "C" void UsageFault_Handler()
{
    blink(6);
}

extern "C" void EXTI0_IRQHandler(void)
{
  gpio_handle_rotation();
  SET_BIT(EXTI->PR, EXTI_PR_PR0_Msk);
}

extern "C" void EXTI1_IRQHandler(void)
{
  gpio_handle_rotation();
  SET_BIT(EXTI->PR, EXTI_PR_PR1_Msk);
}

extern "C" void EXTI9_5_IRQHandler (void)
{
  gpio_handle_imu_interupt();
  SET_BIT(EXTI->PR, EXTI_PR_PR9_Msk);
}

extern "C" void EXTI15_10_IRQHandler(void)
{
  gpio_handle_key();
  SET_BIT(EXTI->PR, EXTI_PR_PR10_Msk);
}

extern "C" void DMA1_Stream0_IRQHandler(void)
{
  dma_isr_rx_handler();
}

extern "C" void DMA1_Stream1_IRQHandler(void)
{
  dma_isr_tx_handler();
}
