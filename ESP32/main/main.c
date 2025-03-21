#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "secrets.h"
#include "wifi.h"


#define LED_PIN 2  // Pin number for the built-in LED



void app_main(void)
{
    nvs_flash_init(); // this is important in wifi case to store configurations , code will not work if this is not added
    wifi_connection();

    xTaskCreate(&wifi_check_task, "wifi_check_task", 2048, NULL, 5, NULL); // FreeRTOS task to check wifi connection

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
