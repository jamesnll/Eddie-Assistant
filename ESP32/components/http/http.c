#include "http.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include <stdio.h>

#define TAG "HTTP_CLIENT"

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
        .url = "https://3605-2604-3d08-9a77-8530-69d0-8433-4aa5-575e.ngrok-free.app",
        .event_handler = _http_event_handler,
    };
}