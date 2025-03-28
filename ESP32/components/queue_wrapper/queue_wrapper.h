#ifndef QUEUE_WRAPPER_H
#define QUEUE_WRAPPER_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <stdbool.h>

// Constants
#define QUEUE_SIZE 5
#define QUEUE_ITEM_SIZE sizeof(char *)

// Extern queue to be used across all tasks
extern QueueHandle_t queue_handle;

void queue_init();
bool queue_send();
bool queue_receive();

#endif // QUEUE_WRAPPER_H