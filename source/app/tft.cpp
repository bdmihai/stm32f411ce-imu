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
 |  Author: Mihai Baneu                           Last modified: 08.Oct.2022  |
 |                                                                            |
 |___________________________________________________________________________*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "stm32f4xx.h"
#include "stm32rtos.h"
#include "queue.h"
#include "mpu6050.h"
#include "gpio.h"
#include "system.h"
#include "tft.h"
#include "spi.h"
#include "st7735.h"


extern const uint8_t u8x8_font_8x13B_1x2_f[];

 /* Queue used to communicate TFT update messages. */
QueueHandle_t tft_queue = NULL;

void tft_init()
{
    tft_queue = xQueueCreate(6, sizeof(tft_event_t));
}
    gpio ttf_res_gpio(GPIOA, 3, gpio::output, gpio::push_pull, gpio::low_speed, gpio::no_pull);
    gpio ttf_dc_gpio(GPIOA, 6, gpio::output, gpio::push_pull, gpio::low_speed, gpio::no_pull);

void tft_run(void *params)
{
    (void)params;

    ttf_res_gpio.init();
    ttf_res_gpio.high();

    ttf_dc_gpio.init();
    ttf_dc_gpio.low();

    st7735_hw_control_t hw = {
        .res_high = []() { ttf_res_gpio.high(); },
        .res_low  = []() { ttf_res_gpio.low(); },
        .dc_high  = []() { ttf_dc_gpio.high(); },
        .dc_low   = []() { ttf_dc_gpio.low(); },
        .data_wr  = spi_write,
        .data_rd  = spi_read,
        .delay_us = delay_us
    };

    st7735_init(hw);

    /* perform a HW reset */
    st7735_hardware_reset();
    st7735_sleep_out_and_booster_on();
    delay_us(10000);
    st7735_normal_mode();

    /* configure the display */
    st7735_display_inversion_off();
    st7735_interface_pixel_format(ST7735_16_PIXEL);
    st7735_display_on();
    st7735_memory_data_access_control(0, 1, 0, 0, 0, 0);
    st7735_column_address_set(0, 128-1);
    st7735_row_address_set(0, 160-1);

    /* prepare the background */
    st7735_draw_fill(0, 0, 160-1, 128-1, st7735_rgb_white);
    st7735_draw_rectangle(3, 3, 157, 125, st7735_rgb_red, st7735_rgb_yellow);

    /* process events */
    for (;;) {
        tft_event_t tft_event;
        char txt[16] = { 0 };

        if (xQueueReceive(tft_queue, &tft_event, portMAX_DELAY) == pdPASS) {
            switch (tft_event.type) {
                case tft_event_motion:
                    sprintf(txt, "ax: %7.3f", tft_event.motion.ax);
                    st7735_draw_string(u8x8_font_8x13B_1x2_f, 4*8, 1*8, st7735_rgb_black, st7735_rgb_yellow, txt);

                    sprintf(txt, "ay: %7.3f", tft_event.motion.ay);
                    st7735_draw_string(u8x8_font_8x13B_1x2_f, 4*8, 3*8, st7735_rgb_black, st7735_rgb_yellow, txt);

                    sprintf(txt, "az: %7.3f", tft_event.motion.az);
                    st7735_draw_string(u8x8_font_8x13B_1x2_f, 4*8, 5*8, st7735_rgb_black, st7735_rgb_yellow, txt);

                    sprintf(txt, "gx: %7.3f", tft_event.motion.gx);
                    st7735_draw_string(u8x8_font_8x13B_1x2_f, 4*8, 7*8, st7735_rgb_black, st7735_rgb_yellow, txt);

                    sprintf(txt, "gy: %7.3f", tft_event.motion.gy);
                    st7735_draw_string(u8x8_font_8x13B_1x2_f, 4*8, 9*8, st7735_rgb_black, st7735_rgb_yellow, txt);

                    sprintf(txt, "gz: %7.3f", tft_event.motion.gz);
                    st7735_draw_string(u8x8_font_8x13B_1x2_f, 4*8,11*8, st7735_rgb_black, st7735_rgb_yellow, txt);

                    sprintf(txt, "%cC: %7.3f", '°', tft_event.motion.temp);
                    st7735_draw_string(u8x8_font_8x13B_1x2_f, 4*8,13*8, st7735_rgb_black, st7735_rgb_yellow, txt);
                    break;
                
                default:
                    break;
            }
        }
    }
}
