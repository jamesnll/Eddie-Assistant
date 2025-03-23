#ifndef WIFI_H
#define WIFI_H

#include <stdio.h> // for basic printf commands
#include <string.h> // for handling strings
#include "freertos/FreeRTOS.h" // for delay,mutexs,semphrs rtos operations
#include "esp_system.h" // esp_init funtions esp_err_t 
#include "esp_wifi.h" // esp_wifi_init functions and wifi operations
#include "esp_log.h" // for showing logs
#include "esp_event.h" // for wifi event
#include "nvs_flash.h" // non volatile storage
#include "lwip/err.h" // light weight ip packets error handling
#include "lwip/sys.h" // system applications for light weight ip apps
#include "../secrets/secrets.h" // wifi secret credentials
#include "freertos/event_groups.h"

void attempt_wifi_reconnect();
void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id,void *event_data);
void wifi_connection();
void wifi_check_task(void *pvParameter);

#define CHECK_WIFI_TASK_INTERVAL_MS 30000  // 30 seconds interval
#define MAX_RETRIES 10                      // Maximum number of retries to reconnect to wifi
#define RETRY_DELAY_MS 2000                // Delay between reconnection attempts

extern EventGroupHandle_t wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0  

#endif // WIFI_H