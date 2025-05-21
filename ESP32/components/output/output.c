#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"
#include "output.h"

#define MP3_BUFFER_SIZE 2048

#define TAG "OUTPUT"
void output_task(void *pvParameters)
{
    // Init minimp3 decoder
    mp3dec_t mp3d;
    mp3dec_init(&mp3d);

    // Declare minimp3 variables
    uint8_t mp3_buf[MP3_BUFFER_SIZE]; // Buffer to store MP3 data
    size_t mp3_buf_len = 0;           // Current length of mp3_buf
    uint8_t *mp3_ptr = NULL;          // mp3_buf pointer

    int16_t pcm[1152 * 2];            // PCM output buffer (max samples per MP3 frame * channels)
    mp3dec_frame_info_t mp3_info;     // Hold metadata of the decoded MP3 frame


    /* TODOs: 
    
    2. Load mp3 from flash
    3. Decode mp3 into PCM using minimp3
    4. Load PCM data into MAX98357 component
    5. Output audio via speaker
    6. Confirm successful output, then delete mp3 data, else try to play audio again
    7. End task
    */
}