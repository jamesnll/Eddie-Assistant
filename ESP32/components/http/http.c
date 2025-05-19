#include "http.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "stream_buf.h"
#include <stdio.h>

#define TAG "HTTP_CLIENT"
#define QUERY_LENGTH 1024

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

        // Check for data and send it into the output stream buffer
        if (evt->data && evt->data_len > 0)
        {
            size_t bytes_sent = xStreamBufferSend(output_stream_buf, evt->data, evt->data_len, pdMS_TO_TICKS(100));

            if (bytes_sent != evt->data_len)
            {
                ESP_LOGI(TAG, "STREAM_BUFFER: Only sent %d out of %d bytes", bytes_sent, evt->data_len);
            } 
            else
            {
                ESP_LOGI(TAG, "STREAM_BUFFER: Wrote %d bytes to output stream buffer", bytes_sent);
            }
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
        }
        else 
        {
            // Otherwise, copy the character as it is
            encoded_query[encoded_iterator++] = query[i];
        }
    }

    // Null terminate the encoded query
    encoded_query[encoded_iterator] = '\0';

    // Copy the encoded string back to the original query
    snprintf(query, QUERY_LENGTH, "%s", encoded_query);
}

void create_full_url(char *base_url, char *base_path, char *user_query, char **result_url)
{
    // Get lengths for the full path and the result url
    size_t full_path_len = strlen(base_path) + strlen(user_query) + 1; // + 1 for '\0'
    size_t result_url_len = strlen(base_url) + full_path_len + 1; // + 1 for '\0'

    // Get full path endpoint
    char *full_path = (char *)malloc(sizeof(char) * full_path_len);
    if (full_path == NULL)
    {
        ESP_LOGI("ERROR", "Malloc error when creating full_path");
        return;
    }

    snprintf(full_path, full_path_len, "%s%s", base_path, user_query);

    // Get result URL
    *result_url = (char *)malloc(sizeof(char) * result_url_len);
    if (*result_url == NULL)
    {
        ESP_LOGI("ERROR", "Malloc error when creating result_url");
        free(full_path);
        return;
    }

    snprintf(*result_url, result_url_len, "%s%s", base_url, full_path);

    free(full_path);
}

// TODO: (Future Feature) Read input from a message queue and append the received data to the config.url
void http_get_task(void *pvParameters)
{
    // Place ngrok url address here
    char base_url[] = "https://ede4-2604-3d08-9a77-8530-5037-97a4-b7e4-9394.ngrok-free.app/";

    // Base path
    char base_path[] = "recommend-shows?query=";

    // User query
    // TODO: Change this to receive the input from a message queue sent by the input task (Future Feature)
    char user_query[] = "Recommend me a show with psychological thriller aspects";

    // Full URL endpoint for recommendations
    char *full_url = NULL;

    // Encode the query
    encode_query(user_query);

    // Create the full url endpoint
    create_full_url(base_url, base_path, user_query, &full_url);

    // Create esp http client config
    esp_http_client_config_t config = {
        .url = full_url,
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
    free(full_url);
    esp_http_client_cleanup(client);
    vTaskDelete(NULL);
}