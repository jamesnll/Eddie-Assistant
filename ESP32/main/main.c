#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "secrets.h"
#include "wifi.h"


#define LED_PIN 2  // Pin number for the built-in LED



void app_main(void)
{
    // Configure the I/O pin for output
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    while (1) {
        // Turn the LED on
        gpio_set_level(LED_PIN, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // Wait for 1 second

        // Turn the LED off
        gpio_set_level(LED_PIN, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // Wait for 1 second
    }
}
