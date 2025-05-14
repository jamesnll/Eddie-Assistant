#ifndef STREAM_BUFFER_H
#define STREAM_BUFFER_H

#include "freertos/stream_buffer.h"

extern StreamBufferHandle_t input_stream_buffer;  // Input to HTTP
extern StreamBufferHandle_t output_stream_buffer; // HTTP to output


#endif // STREAM_BUFFER_H