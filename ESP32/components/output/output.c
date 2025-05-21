#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"
#include "output.h"

#define TAG "OUTPUT"
void output_task(void *pvParameters)
{
    /* TODOs: 
    
    2. Load mp3 from flash
    3. Decode mp3 into PCM using minimp3
    4. Load PCM data into MAX98357 component
    5. Output audio via speaker
    6. Confirm successful output, then delete mp3 data, else try to play audio again
    7. End task
    */
}