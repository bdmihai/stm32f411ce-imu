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
 
#pragma once

/* initialization */
void gpio_init();

void gpio_handle_rotation();
void gpio_handle_key();

void gpio_handle_imu_interupt();


class gpio {

    public: 
        typedef enum {
            input = 0, output = 1, alt = 2
        } gpio_mode;

        typedef enum {
            push_pull = 0, open_drain = 1
        } gpio_otype;

        typedef enum {
            low_speed = 0, medium_speed = 1, fast_speed = 2, high_speed = 3
        } gpio_ospeed;

        typedef enum {
            no_pull = 0, pull_up = 1, pull_down = 2
        } gpio_pupd;

        gpio(GPIO_TypeDef *port, uint8_t pin, gpio_mode mode, gpio_otype otype, gpio_ospeed ospeed, gpio_pupd pupd);

        void init();

        void high();
        void low();
        void toggle();
        uint32_t value();

    private:
        GPIO_TypeDef *m_port;
        uint8_t m_pin;
        gpio_mode m_mode;
        gpio_otype m_otype;
        gpio_ospeed m_ospeed;
        gpio_pupd m_pupd;
};