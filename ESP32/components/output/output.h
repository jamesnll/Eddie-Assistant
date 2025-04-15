#ifndef OUTPUT_H
#define OUTPUT_H

#include "queue_wrapper.h"
#include "esp_log.h"

#define BUFFER_SIZE 1024

// Function prototype to display http response to terminal and output via speaker
void output_task(void *pvParameters);

#endif // OUTPUT_H