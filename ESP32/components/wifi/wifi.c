#include "wifi.h"

uint32_t retry_num = 0; // number of retries to reconnect to wifi

void attempt_wifi_reconnect()
{
    if (retry_num < MAX_RETRIES)
    {
        esp_wifi_connect();
        retry_num++;
        printf("Attempting to reconnect... (%ld/%d)\n", retry_num, MAX_RETRIES);
        vTaskDelay(RETRY_DELAY_MS / portTICK_PERIOD_MS); // Delay before retrying to prevent rapid reconnection attempts
    }
    else
    {
        printf("Max reconnection attempts reached.\n");
    }
}

void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id,void *event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        printf("WIFI CONNECTING....\n");
        break;
    case WIFI_EVENT_STA_CONNECTED:
        printf("WiFi CONNECTED\n");
        retry_num = 0; // Reset retry count when connected
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        printf("WiFi lost connection\n");
        attempt_wifi_reconnect();
        break;
    case IP_EVENT_STA_GOT_IP:
        printf("Wifi got IP...\n\n");
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);  // Set the event bit
        break;
    default:
        break;
    }
}

void wifi_connection()
{
    wifi_config_t wifi_configuration = {
        .sta= {
            .ssid = "",
            .password = "" // const char are being sent for both fields, so we leave them blank and use strncpy
        }
    };

    esp_netif_init(); // network interface initialization
    esp_event_loop_create_default(); // responsible for handling and dispatching events
    esp_netif_create_default_wifi_sta(); // sets up necessary data structures for wifi station interface
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT(); // sets up wifi_init_config struct with default values
    esp_wifi_init(&wifi_initiation); // wifi initialized with default wifi_initiation
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL); // creating event handler register for wifi
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL); // creating event handler register for ip event
    strncpy((char *)wifi_configuration.sta.ssid, SECRET_SSID, strlen(SECRET_SSID) + 1); // copy chars from hardcoded configs to struct
    strncpy((char *)wifi_configuration.sta.password, SECRET_PASSWORD, strlen(SECRET_PASSWORD) + 1);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration); // setting up configs when event ESP_IF_WIFI_STA
    esp_wifi_start(); // start connection with configs provided in function
    esp_wifi_set_mode(WIFI_MODE_STA); // station mode selected
    esp_wifi_connect(); // connect with saved ssid and password
    printf("wifi_init_softap finished.\nSSID: %s\nPassword: %s\n", SECRET_SSID, SECRET_PASSWORD);
}

// FreeRTOS task to check Wi-Fi status and reconnect if necessary
void wifi_check_task(void *pvParameter) {
    while (1) {
        wifi_ap_record_t ap_info;
        if (esp_wifi_sta_get_ap_info(&ap_info) != ESP_OK) {
            printf("Wi-Fi is disconnected, attempting to reconnect...\n");
            attempt_wifi_reconnect();
        } else {
            printf("Wi-Fi is connected.\n");
        }
        vTaskDelay(CHECK_WIFI_TASK_INTERVAL_MS / portTICK_PERIOD_MS); // Wait for 30 seconds
    }
}