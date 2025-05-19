#ifndef STREAM_BUF_H
#define STREAM_BUF_H

#include "freertos/FreeRTOS.h"
#include "freertos/stream_buffer.h"

extern StreamBufferHandle_t input_stream_buffer;  // Input to HTTP
extern StreamBufferHandle_t output_stream_buffer; // HTTP to output

int init_input_stream_buffer(void);
int init_output_stream_buffer(void);

#endif // STREAM_BUFFER_H