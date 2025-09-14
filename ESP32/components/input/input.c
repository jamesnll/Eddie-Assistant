/*

Setup MIC
Read values from mic

*/

#include "input.h"


static const char *TAG = "INPUT";

esp_err_t input_init(void)
{
    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_RX,              // Master, RX mode
        .sample_rate = I2S_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE,             // 32-bit date
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,        // Mono (Left Channel)
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,  // Bit format
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,           // Interrupt level 1
        .dma_buf_count = 4,                                 // Number of DMA buffers
        .dma_buf_len = I2S_BUFFER_SIZE,                     // Buffer length
        .use_apll = false                                   // No Audio PLL
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_SCK,
        .ws_io_num = I2S_WS,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = I2S_SD
    };


    return ESP_OK;
}