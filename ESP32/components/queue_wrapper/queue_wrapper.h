#ifndef QUEUE_WRAPPER_H
#define QUEUE_WRAPPER_H

#include "freertos/queue.h"


void queue_init();
bool queue_send();
bool queue_receive();

#endif // QUEUE_WRAPPER_H