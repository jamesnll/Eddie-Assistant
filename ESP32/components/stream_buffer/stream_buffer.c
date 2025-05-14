#include "stream_buffer.h"

// Macros
#define INPUT_STREAM_BUFFER_SIZE  1024
#define OUTPUT_STREAM_BUFFER_SIZE 1024
#define TRIGGER_LEVEL             1

// Stream buffer definitions
StreamBufferHandle_t input_stream_buffer = NULL;
StreamBufferHandle_t output_stream_buffer = NULL;

int init_input_stream_buffer(void)
{
    // Check if input_stream_buffer isn't already created
    if (!input_stream_buffer)
    {
        // Create the buffer
        input_stream_buffer = xStreamBufferCreate(INPUT_STREAM_BUFFER_SIZE, TRIGGER_LEVEL);
        // Error check
        if (!input_stream_buffer)
        {
            printf("Error creating input stream buffer!\n");
            return -1;
        }
    }

    return 0;
}

int init_output_stream_buffer(void)
{
    // Check if output_stream_buffer isn't already created
    if (!output_stream_buffer)
    {
        // Create the buffer
        output_stream_buffer = xStreamBufferCreate(OUTPUT_STREAM_BUFFER_SIZE, TRIGGER_LEVEL);
        // Error check
        if (!output_stream_buffer)
        {
            printf("Error creating output stream buffer!\n");
            return -1;
        }
    }

    return 0;
}