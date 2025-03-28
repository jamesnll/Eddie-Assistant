#ifndef QUEUE_WRAPPER_H
#define QUEUE_WRAPPER_H

#include "freertos/queue.h"

// Extern queue to be used across all tasks
extern QueueHandle_t queue_handle;

void queue_init();
bool queue_send();
bool queue_receive();

#endif // QUEUE_WRAPPER_H