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
#include "task.h"
#include "queue.h"
#include "system.h"
#include "gpio.h"
#include "isr.h"
#include "i2c.h"
#include "spi.h"
#include "led.h"
#include "dma.h"
#include "mpu6050.h"
#include "tft.h"
#include "rencoder.h"

TaskHandle_t motion_run_task = 0;
mpu6050_data_t motion;

static void user_handler(void *pvParameters)
{
    (void)pvParameters;

    for (;;) {
        rencoder_output_event_t event;
        if (xQueueReceive(rencoder_output_queue, &event, portMAX_DELAY) == pdPASS) {
            switch(event.type) {
                case rencoder_output_rotation:
                    break;
                case rencoder_output_key:
                    if (event.key == RENCODER_KEY_RELEASED) {
                        rencoder_reset();
                    }
                    break;
            }
        }
    }
}

static void update_run(void *pvParameters)
{
    (void)pvParameters;

    vTaskDelay(100 / portTICK_PERIOD_MS);

    for (;;) {
        TickType_t xLastWakeTime = xTaskGetTickCount();

        tft_event_t tft_event;
        tft_event.type = tft_event_motion;
        tft_event.motion = motion;
        xQueueSendToBack(tft_queue, &tft_event, (TickType_t) 1);

        vTaskDelayUntil( &xLastWakeTime, 150 / portTICK_PERIOD_MS );
    }
}

static void motion_run(void *pvParameters)
{
    (void)pvParameters;

    mpu6050_hw_control_t hw = {
        .data_wr = i2c_write,
        .data_rd = i2c_read,
        .address = MPU6050_ADDRESS_AD0_LOW
    };

    vTaskDelay(100 / portTICK_PERIOD_MS);
    mpu6050_init(hw);

    for (;;) {
        // wait to be notified of an interrupt
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY );

        // get the data from the IMU
        motion = mpu6050_get_motion();
    }
}

int main(void)
{
    /* initialize the system */
    system_init();

    /* initialize the gpio */
    gpio_init();

    /* initialize the interupt service routines */
    isr_init();

    /* initialize the i2c interface */
    i2c_init();

    /* initialize the spi interface */
    spi_init();

    /* initialize the dma controller */
    dma_init();

    /* init led handler */
    led_init();

    /* init lcd display */
    tft_init();

    /* initialize the encoder */
    rencoder_init(0, 100);

    /* create the tasks specific to this application. */
    xTaskCreate(led_run,      "led",          configMINIMAL_STACK_SIZE,     NULL, 3, NULL);
    xTaskCreate(tft_run,      "tft",          configMINIMAL_STACK_SIZE*2,   NULL, 2, NULL);
    xTaskCreate(dma_run,      "dma",          configMINIMAL_STACK_SIZE*2,   NULL, 2, NULL);
    xTaskCreate(rencoder_run, "rencoder",     configMINIMAL_STACK_SIZE,     NULL, 2, NULL);
    xTaskCreate(user_handler, "user_handler", configMINIMAL_STACK_SIZE*2,   NULL, 2, NULL);
    xTaskCreate(update_run,   "update_run",   configMINIMAL_STACK_SIZE*2,   NULL, 2, NULL);
    xTaskCreate(motion_run,   "motion_run",   configMINIMAL_STACK_SIZE*2,   NULL, 9, &motion_run_task);

    /* start the scheduler. */
    vTaskStartScheduler();

    /* should never get here ... */
    blink(10);
    return 0;
}
