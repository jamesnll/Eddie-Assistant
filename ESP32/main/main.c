#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "secrets.h"
#include "wifi.h"
#include "http.h"
#include "queue_wrapper.h"
#include "stream_buf.h"
#include "output.h"

#define TAG "MAIN"


#define LED_PIN 2  // Pin number for the built-in LED

#define WIFI_CONNECTED_BIT BIT0  // Define the event bit for Wi-Fi connection

EventGroupHandle_t wifi_event_group;  // Event group handle

void app_main(void)
{
    nvs_flash_init(); // this is important in wifi case to store configurations , code will not work if this is not added

    wifi_event_group = xEventGroupCreate();

    wifi_connection();

    xTaskCreate(&wifi_check_task, "wifi_check_task", 2048, NULL, 5, NULL); // FreeRTOS task to check wifi connection

    // Wait for the Wi-Fi connection before starting the HTTP task
    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);  // Wait for the Wi-Fi connection bit

    // Initialize the output stream buf
    if (init_output_stream_buf() == -1)
    {
        return;
    }

    // Create the queue
    queue_init();

    // Start the HTTP GET task after Wi-Fi is connected
    ESP_LOGI(TAG, "Free heap before I2S init: %" PRIu32 " bytes", esp_get_free_heap_size());
    xTaskCreate(&http_get_task, "http_get_task", 12288, NULL, 5, NULL);

    vTaskDelay(2000 / portTICK_PERIOD_MS);  // Wait for 5 seconds to send the HTTP Request (Fix later, have task created without delay then use sleep cycle)

    // Start the Output task
    ESP_LOGI(TAG, "Free heap before I2S init: %" PRIu32 " bytes", esp_get_free_heap_size());
    xTaskCreate(&output_task, "output_task", 20480, NULL, 6, NULL);

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
