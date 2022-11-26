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

#pragma once

typedef enum {
    dma_request_type_i2c_write,
    dma_request_type_i2c_read,
} dma_request_type;

typedef enum {
    dma_request_status_success,
    dma_request_status_error,
} dma_response_status;

typedef struct dma_request_event_t {
    dma_request_type type;
    uint8_t address;
    uint8_t buffer[16];
    uint8_t length;
} dma_request_event_t;

typedef struct dma_response_event_t {
    dma_response_status status;
    uint8_t buffer[16];
    uint8_t length;
} dma_response_event_t;

typedef struct dma_buffer_t {
    uint8_t buffer[16];
    uint8_t length;
} dma_buffer_t;

extern QueueHandle_t dma_request_queue;
extern QueueHandle_t dma_response_queue;

void dma_init();
void dma_isr_rx_handler();
void dma_isr_tx_handler();
void dma_run(void *pvParameters);
