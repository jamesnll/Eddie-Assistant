#ifndef OUTPUT_H
#define OUTPUT_H

#include "stream_buf.h"
#include "esp_log.h"

// Function prototype to display http response to terminal and output via speaker
void output_task(void *pvParameters);

#endif // OUTPUT_H