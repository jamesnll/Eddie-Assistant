#include "http.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include <stdio.h>

#define TAG "HTTP_CLIENT"

#define MAX_HTTP_OUTPUT_BUFFER 1024

static char response_buffer[MAX_HTTP_OUTPUT_BUFFER];

// TODO: Create a function to handle http events
// TODO: Implement functionality for HTTP events
static esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
        ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADERS_SENT:
        ESP_LOGI(TAG, "HTTP_EVENT_HEADERS_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA");
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    default:
        break;
    }

    return ESP_OK;
}

// TODO: Implement functionality for the http_get_task function
void http_get_task(void *pvParameters)
{
    // Create esp http client config
    esp_http_client_config_t config = 
    {
        .url = "http://3605-2604-3d08-9a77-8530-69d0-8433-4aa5-575e.ngrok-free.app/",
        .event_handler = _http_event_handler,
        .user_data = response_buffer
    };

    // Initialize HTTP Client
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // Perform GET request
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %lld",
                 esp_http_client_get_status_code(client),
                 (long long)esp_http_client_get_content_length(client));
        ESP_LOGI(TAG, "Response: %s", response_buffer);
    }
    else
    {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }

    // Cleanup and delete task
    esp_http_client_cleanup(client);
    vTaskDelete(NULL);
}