#ifndef HTTP_H
#define HTTP_H

#include "esp_err.h"


// Function prototype for a FreeRTOS HTTP GET task
void http_get_task(void *pvParameters);


#endif // HTTP_H