#ifndef INPUT_H
#define INPUT_H

#include "driver/i2s.h"
#include "esp_err.h"

// I2S Pins
#define I2S_WS 25
#define I2S_SCK 33
#define I2S_SD 32

// I2S Config
#define I2S_SAMPLE_RATE 16000
#define I2S_PORT_NUM (1)
#define I2S_BITS_PER_SAMPLE I2S_DATA_BIT_WIDTH_32BIT
#define I2S_BUFFER_SIZE 1024

esp_err_t input_init(void);
void input_task(void *pvParameters);

#endif // INPUT_H