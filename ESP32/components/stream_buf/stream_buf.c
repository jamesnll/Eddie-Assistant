#include "stream_buf.h"
#include <stdio.h>

// Macros
#define INPUT_STREAM_BUF_SIZE  1024
#define OUTPUT_STREAM_BUF_SIZE 32768
#define TRIGGER_LEVEL             1

// Stream buffer definitions
StreamBufferHandle_t input_stream_buf = NULL;
StreamBufferHandle_t output_stream_buf = NULL;

int init_input_stream_buf(void)
{
    // Check if input_stream_buf isn't already created
    if (!input_stream_buf)
    {
        // Create the buf
        input_stream_buf = xStreamBufferCreate(INPUT_STREAM_BUF_SIZE, TRIGGER_LEVEL);
        // Error check
        if (!input_stream_buf)
        {
            printf("Error creating input stream buf!\n");
            return -1;
        }
    }

    return 0;
}

int init_output_stream_buf(void)
{
    // Check if output_stream_buf isn't already created
    if (!output_stream_buf)
    {
        // Create the buf
        output_stream_buf = xStreamBufferCreate(OUTPUT_STREAM_BUF_SIZE, TRIGGER_LEVEL);
        // Error check
        if (!output_stream_buf)
        {
            printf("Error creating output stream buf!\n");
            return -1;
        }
    }

    return 0;
}