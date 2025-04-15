#include "output.h"

#define TAG "OUTPUT"
void output_task(void *pvParameters)
{
    // Phase 1: Output response to terminal
    // TODO: Read HTTP response from the queue
    char *buffer = NULL;
    if (queue_receive(&buffer) == pdPASS && buffer != NULL)
    {
        ESP_LOGI(TAG, "%s", buffer);
    }
    else
    {
        ESP_LOGI(TAG, "Failed to receive buffer from queue.");
    }
    // TODO: Process JSON response to retrieve the show recommendations (this should be a seperate function -> either in output or http directory)
    // TODO: Output the processed response to the terminal

    vTaskDelete(NULL); // TODO: Change this to put the task to sleep, this task gets woken up once http task finishes putting response in the queue

    // Phase 2:
    // TODO: Wire up hardware to allow the speaker to output response
    // TODO: Include libraries needed for TTS (ESP-TTS)
    // TODO: Output the processed response via speaker
}