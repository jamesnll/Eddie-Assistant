#include "queue_wrapper.h"

void queue_init()
{
    // Only create a single instance
    if (queue_handle == NULL)
    {
        queue_handle = xQueueCreate(QUEUE_SIZE, QUEUE_ITEM_SIZE);

        if (queue_handle == NULL)
        {
            printf("Failed to create queue.\n");
        }
    }
    else
    {
        printf("Queue already created.\n");
    }
}

int queue_send(char *msg)
{
    int result;

    result = xQueueSend(queue_handle, &msg, (TickType_t) 0);

    return result;
}