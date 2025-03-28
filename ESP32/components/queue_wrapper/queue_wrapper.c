#include "queue_wrapper.h"

void queue_init()
{
    // TODO: Turn this into a singleton if possible
    queue_handle = xQueueCreate(QUEUE_SIZE, QUEUE_ITEM_SIZE);

    if (queue_handle == NULL)
    {
        printf("Failed to create queue.\n");
    }
}