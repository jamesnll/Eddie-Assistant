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
        /* code */
        break;
    case HTTP_EVENT_ON_CONNECTED:
        /* code */
        break;
    case HTTP_EVENT_HEADERS_SENT:
        /* code */
        break;
    case HTTP_EVENT_ON_HEADER:
        /* code */
        break;
    case HTTP_EVENT_ON_DATA:
        /* code */
        break;
    case HTTP_EVENT_ON_FINISH:
        /* code */
        break;
    case HTTP_EVENT_DISCONNECTED:
        /* code */
        break;
    default:
        break;
    }

    return ESP_OK;
}

// TODO: Implement functionality for the http_get_task function