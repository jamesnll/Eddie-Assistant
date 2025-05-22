#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"
#include "output.h"

#define MP3_BUFFER_SIZE 2048

#define TAG "OUTPUT"

static void decode_and_play_mp3_stream(void)
{
    // Init minimp3 decoder
    mp3dec_t mp3d;
    mp3dec_init(&mp3d);

    // Declare minimp3 variables
    uint8_t mp3_buf[MP3_BUFFER_SIZE]; // Buffer to store MP3 data
    size_t mp3_buf_len = 0;           // Current length of mp3_buf
    uint8_t *mp3_ptr = NULL;          // mp3_buf pointer

    int16_t pcm[1152 * 2];            // PCM output buffer (max samples per MP3 frame * channels)
    mp3dec_frame_info_t mp3_info;     // Hold metadata of the decoded MP3 frame

    // Functionality to decode the MP3 data from output_stream_buf
    while (1)
    {
        // Block until at least some data is available in output_stream_buf
        size_t bytes_read = xStreamBufferReceive(output_stream_buf, mp3_buf + mp3_buf_len, sizeof(mp3_buf) - mp3_buf_len, portMAX_DELAY);

        if (bytes_read == 0)
        {
            // Rare case if portMAX_DELAY doesn't work
            ESP_LOGW(TAG, "Stream buffer receive returned 0 bytes.");
            continue;
        }

        // Update mp3_buf_len and ptr
        mp3_buf_len += bytes_read;
        mp3_ptr = mp3_buf;

        // Decode all full frames found in mp3_buf
        while (mp3_buf_len > 0)
        {
            int samples = mp3dec_decode_frame(&mp3d, mp3_ptr, mp3_buf_len, pcm, &mp3_info);

            if (mp3_info.frame_bytes == 0)
            {
                // Not enough data for a full frame, wait for more data
                break;
            }

            if (samples > 0)
            {
                ESP_LOGI(TAG, "Decoded %d samples @ %d Hz, %d channels.",
                        samples, mp3_info.hz, mp3_info.channels);

                /* TODOs: 
                Load PCM data into MAX98357 component
                Output audio via speaker
                */
            }

            // Move ptr past the decoded frame
            mp3_ptr += mp3_info.frame_bytes;
            mp3_buf_len -= mp3_info.frame_bytes;

            // Move remaining partial data to the beginning of mp3_buf
            if (mp3_buf_len > 0 && mp3_ptr != mp3_buf)
            {
                memmove(mp3_buf, mp3_ptr, mp3_buf_len);
            }
        }
    }
}

// FreeRTOS task that handles decoding and outputting MP3 via minimp3 and I2S
void output_task(void *pvParameters)
{
    // Decode MP3 from output_stream_buf
    decode_and_play_mp3_stream();

    // End the task
    vTaskDelete(NULL);
}
