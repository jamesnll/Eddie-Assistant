#include "http.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include <stdio.h>

#define TAG "HTTP_CLIENT"
#define QUERY_LENGTH 1024

// TODO: Once we receive the data, send it into a message queue to be used by another FreeRTOS task (Future feature)
static esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
        ESP_LOGI(TAG, "HTTP_EVENT_ERROR: General error occurred.");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED: Connection established.");
        break;
    case HTTP_EVENT_HEADERS_SENT:
        ESP_LOGI(TAG, "HTTP_EVENT_HEADERS_SENT: Headers successfully sent.");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER: Received headers successfully.");
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA: Received %d bytes.", evt->data_len);

        // Print received data
        // TODO: Modify this to send it to a message queue (can keep this for logging, for example: %s was written to the message queue)
        if (evt->data && evt->data_len > 0) { 
            ESP_LOGI(TAG, "Received Data: %.*s", evt->data_len, (char*)evt->data);
        }
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH: HTTP Request completed!");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED: Connection closed unexpectedly.");
        break;
    default:
        break;
    }

    return ESP_OK;
}

void encode_query(char *query)
{
    char encoded_query[QUERY_LENGTH];
    int encoded_iterator = 0;

    // Iterate through query
    for (int i = 0; query[i] != '\0'; ++i)
    {
        if (query[i] == ' ') 
        {
            // If space is found, replace with "%20"
            encoded_query[encoded_iterator++] = '%';
            encoded_query[encoded_iterator++] = '2';
            encoded_query[encoded_iterator++] = '0';
        } else 
        {
            // Otherwise, copy the character as it is
            encoded_query[encoded_iterator++] = query[i];
        }
    }

    // Null terminate the encoded query
    encoded_query[encoded_iterator] = '\0';

    // Copy the encoded string back to the original query
    strncpy(query, encoded_query, encoded_iterator);
}

// TODO: Create a string "query" that stores the user query to the recommendation service
// TODO: Append the "query" string to the config.url
// TODO: (Future Feature) Read input from a message queue and append the received data to the config.url
void http_get_task(void *pvParameters)
{
    // User query
    // TODO: Change this to receive the input from a message queue sent by the input task (Future Feature)
    char *user_query = "Recommend me a show with psychological thriller aspects";

    // Create esp http client config
    esp_http_client_config_t config = {
        .url = "https://5001-2604-3d08-9a77-8530-11bb-4c83-cd8f-911.ngrok-free.app",
        .event_handler = _http_event_handler,
        .auth_type = HTTP_AUTH_TYPE_NONE,
        .cert_pem = NULL,  // No certificate PEM (self-signed or invalid cert)
        .skip_cert_common_name_check = true, // Skip Common Name check
        .transport_type = HTTP_TRANSPORT_OVER_SSL
    };

    // Initialize HTTP Client
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // Perform GET request
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) 
    {
        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %lld",
                 esp_http_client_get_status_code(client),
                 (long long)esp_http_client_get_content_length(client));
    }
    else
    {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }

    // Cleanup and delete task
    esp_http_client_cleanup(client);
    vTaskDelete(NULL);
}