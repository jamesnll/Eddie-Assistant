#ifndef STREAM_BUF_H
#define STREAM_BUF_H

#include "freertos/FreeRTOS.h"
#include "freertos/stream_buffer.h"

extern StreamBufferHandle_t input_stream_buf;  // Input to HTTP
extern StreamBufferHandle_t output_stream_buf; // HTTP to output

int init_input_stream_buf(void);
int init_output_stream_buf(void);

#endif // STREAM_BUF_H