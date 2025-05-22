#define MINIMP3_IMPLEMENTATION

// Component Includes
#include "minimp3.h"
#include "driver/i2s.h"
#include "output.h"

// I2S Macros
#define I2S_NUM         (0)
#define I2S_SAMPLE_RATE 44100
#define I2S_BCK_IO      GPIO_NUM_26   // Bit Clock pin
#define I2S_WS_IO       GPIO_NUM_25   // Word Select (LRCLK) pin
#define I2S_DO_IO       GPIO_NUM_22   // Data Out pin

#define MP3_BUFFER_SIZE 2048

#define TAG "OUTPUT"

static void i2s_init()
{
    i2s_config_t i2s_config = 
    {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX, // Master, TX only
        .sample_rate = I2S_SAMPLE_RATE, // Default num, will be changed with decoded sample rate later
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT, // Stereo channel
        .communication_format = I2S_COMM_FORMAT_I2S_MSB,
        .intr_alloc_flags = 0, // Default interrupt priority
        .dma_buf_count = 8,
        .dma_buf_len = 1024,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };

    i2s_pin_config_t pin_config = 
    {
        .bck_io_num = I2S_BCK_IO,
        .ws_io_num = I2S_WS_IO,
        .data_out_num = I2S_DO_IO,
        .data_in_num = -1 // Not used
    };

    // Install and start I2S driver
    i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM, &pin_config);
    i2s_zero_dma_buffer(I2S_NUM);
}

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

                // Update sample rate to decoded sample
                i2s_set_sample_rates(I2S_NUM_0, mp3_info.hz);

                // Output PCM to I2S
                size_t bytes_written = 0;
                i2s_write(I2S_NUM_0, pcm,
                          samples * mp3_info.channels * sizeof(int16_t),
                          &bytes_written, portMAX_DELAY);

                ESP_LOGD(TAG, "Wrote %u bytes to I2S", bytes_written);
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
    // Initialize I2S driver
    i2s_init();

    // Decode MP3 from output_stream_buf
    decode_and_play_mp3_stream();

    // End the task
    vTaskDelete(NULL);
}
