#define MINIMP3_IMPLEMENTATION

// Component Includes
#include "minimp3.h"
#include "driver/i2s.h"
#include "output.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/stream_buffer.h"
#include "esp_log.h"
#include <string.h>
#include <stdlib.h>

// I2S Macros
#define I2S_NUM         (0)
#define I2S_SAMPLE_RATE 44100
#define I2S_BCK_IO      GPIO_NUM_27   // Bit Clock pin (BCLK)
#define I2S_WS_IO       GPIO_NUM_14   // Word Select (LRCLK) pin
#define I2S_DO_IO       GPIO_NUM_26   // Data Out pin (DIN on MAX98357A)

// Optimized buffer sizes
#define MP3_BUFFER_SIZE 8192          // Larger buffer for better frame detection
#define PCM_BUFFER_SIZE (MINIMP3_MAX_SAMPLES_PER_FRAME * 2)  // *2 for stereo
#define MIN_MP3_FRAME_SIZE 32         // Minimum possible MP3 frame size
#define STREAM_READ_SIZE 1024         // Read this much at a time from stream
#define MAX_CONSECUTIVE_ERRORS 5      // Reduced from 10 for faster recovery

#define TAG "OUTPUT"

// Global buffers allocated once (not in loop)
static uint8_t *g_mp3_buf = NULL;
static int16_t *g_pcm_buf = NULL;
static mp3dec_t *g_mp3d = NULL;
static mp3dec_frame_info_t *g_mp3_info = NULL;

// Debug counters
static uint32_t stream_read_attempts = 0;
static uint32_t stream_read_successes = 0;
static uint32_t frames_decoded = 0;

static esp_err_t i2s_init(void)
{
    // More robust I2S configuration
    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,
        .sample_rate = I2S_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,           // Reduced for memory efficiency
        .dma_buf_len = 1024,          // Good balance
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_BCK_IO,
        .ws_io_num = I2S_WS_IO,
        .data_out_num = I2S_DO_IO,
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    esp_err_t ret = i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to install I2S driver: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = i2s_set_pin(I2S_NUM, &pin_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set I2S pins: %s", esp_err_to_name(ret));
        i2s_driver_uninstall(I2S_NUM);
        return ret;
    }

    ret = i2s_zero_dma_buffer(I2S_NUM);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to zero DMA buffer: %s", esp_err_to_name(ret));
    }

    ESP_LOGI(TAG, "I2S initialized successfully");
    return ESP_OK;
}

static bool allocate_buffers(void)
{
    // Allocate all buffers once at startup
    g_mp3_buf = (uint8_t*)heap_caps_malloc(MP3_BUFFER_SIZE, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    g_pcm_buf = (int16_t*)heap_caps_malloc(PCM_BUFFER_SIZE * sizeof(int16_t), MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    g_mp3d = (mp3dec_t*)heap_caps_malloc(sizeof(mp3dec_t), MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    g_mp3_info = (mp3dec_frame_info_t*)heap_caps_malloc(sizeof(mp3dec_frame_info_t), MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    
    if (!g_mp3_buf || !g_pcm_buf || !g_mp3d || !g_mp3_info) {
        ESP_LOGE(TAG, "Failed to allocate buffers");
        return false;
    }
    
    // Initialize buffers
    memset(g_mp3_buf, 0, MP3_BUFFER_SIZE);
    memset(g_pcm_buf, 0, PCM_BUFFER_SIZE * sizeof(int16_t));
    mp3dec_init(g_mp3d);
    
    ESP_LOGI(TAG, "Buffers allocated successfully - MP3: %p, PCM: %p", (void*)g_mp3_buf, (void*)g_pcm_buf);
    return true;
}

static void free_buffers(void)
{
    if (g_mp3_buf) { free(g_mp3_buf); g_mp3_buf = NULL; }
    if (g_pcm_buf) { free(g_pcm_buf); g_pcm_buf = NULL; }
    if (g_mp3d) { free(g_mp3d); g_mp3d = NULL; }
    if (g_mp3_info) { free(g_mp3_info); g_mp3_info = NULL; }
}

static void dump_hex_data(const uint8_t *data, size_t len, const char *label)
{
    if (len > 16) len = 16;  // Limit dump size
    
    char hex_str[64] = {0};
    for (size_t i = 0; i < len; i++) {
        sprintf(hex_str + (i * 3), "%02X ", data[i]);
    }
    ESP_LOGI(TAG, "%s: %s", label, hex_str);
}

static bool find_mp3_sync(uint8_t *buf, size_t buf_size, size_t *sync_pos)
{
    if (!buf || buf_size < 4) return false;
    
    for (size_t i = 0; i <= buf_size - 4; i++) {
        // Look for MP3 sync word (11 bits set)
        if (buf[i] == 0xFF && (buf[i + 1] & 0xE0) == 0xE0) {
            // Additional validation - check if this looks like a real header
            uint8_t version = (buf[i + 1] >> 3) & 0x03;
            uint8_t layer = (buf[i + 1] >> 1) & 0x03;
            uint8_t bitrate = (buf[i + 2] >> 4) & 0x0F;
            uint8_t sampling = (buf[i + 2] >> 2) & 0x03;
            
            // Basic validation - fixed the condition logic
            if (version != 1 && layer == 1 && bitrate != 0 && bitrate != 15 && sampling != 3) {
                *sync_pos = i;
                ESP_LOGI(TAG, "Found valid MP3 sync at position %zu", i);
                return true;
            }
        }
    }
    return false;
}

static size_t read_stream_data(uint8_t *buffer, size_t max_size, size_t current_used)
{
    // Safety checks
    if (!buffer || current_used >= max_size) {
        ESP_LOGE(TAG, "Invalid buffer params: buf=%p, used=%zu, max=%zu", buffer, current_used, max_size);
        return 0;
    }
    
    size_t free_space = max_size - current_used;
    if (free_space < MIN_MP3_FRAME_SIZE) {
        return 0; // Not enough space
    }
    
    size_t available = xStreamBufferBytesAvailable(output_stream_buf);
    if (available == 0) {
        return 0; // No data available
    }
    
    size_t to_read = (available > STREAM_READ_SIZE) ? STREAM_READ_SIZE : available;
    if (to_read > free_space) {
        to_read = free_space;
    }
    
    // Additional safety check
    if (current_used + to_read > max_size) {
        ESP_LOGE(TAG, "Buffer overflow prevented: used=%zu, to_read=%zu, max=%zu", current_used, to_read, max_size);
        return 0;
    }
    
    stream_read_attempts++;
    size_t bytes_read = xStreamBufferReceive(
        output_stream_buf,
        buffer + current_used,
        to_read,
        pdMS_TO_TICKS(50)  // Reasonable timeout
    );
    
    if (bytes_read > 0) {
        stream_read_successes++;
        ESP_LOGI(TAG, "Read %zu bytes from stream (available: %zu)", bytes_read, available);
        
        // Debug first few reads
        if (stream_read_successes <= 3) {
            dump_hex_data(buffer + current_used, bytes_read, "Stream data");
        }
    }
    
    return bytes_read;
}

static void decode_and_play_mp3_stream(void)
{
    size_t mp3_buf_used = 0;
    bool first_frame = true;
    uint32_t current_sample_rate = I2S_SAMPLE_RATE;
    size_t consecutive_errors = 0;
    bool sync_found = false;
    uint32_t loop_count = 0;

    ESP_LOGI(TAG, "Starting MP3 decode loop");
    
    while (1) {
        loop_count++;
        
        // Periodic status
        if (loop_count % 1000 == 0) {
            size_t available = xStreamBufferBytesAvailable(output_stream_buf);
            ESP_LOGI(TAG, "Loop %lu: Buffer used: %zu, Stream available: %zu, Frames: %lu, Errors: %zu", 
                     loop_count, mp3_buf_used, available, frames_decoded, consecutive_errors);
        }
        
        // Reset on too many errors
        if (consecutive_errors > MAX_CONSECUTIVE_ERRORS) {
            ESP_LOGW(TAG, "Too many errors, resetting decoder");
            mp3_buf_used = 0;
            consecutive_errors = 0;
            sync_found = false;
            mp3dec_init(g_mp3d);
            continue;
        }
        
        // Read more data if needed
        if (mp3_buf_used < MP3_BUFFER_SIZE / 2) {
            size_t bytes_read = read_stream_data(g_mp3_buf, MP3_BUFFER_SIZE, mp3_buf_used);
            mp3_buf_used += bytes_read;
        }
        
        // Need minimum data
        if (mp3_buf_used < MIN_MP3_FRAME_SIZE) {
            vTaskDelay(pdMS_TO_TICKS(20));
            continue;
        }
        
        // Find sync if not found yet
        if (!sync_found) {
            size_t sync_pos;
            if (find_mp3_sync(g_mp3_buf, mp3_buf_used, &sync_pos)) {
                if (sync_pos > 0) {
                    ESP_LOGI(TAG, "Found MP3 sync at position %zu", sync_pos);
                    // Safety check before memmove
                    if (sync_pos < mp3_buf_used) {
                        mp3_buf_used -= sync_pos;
                        memmove(g_mp3_buf, g_mp3_buf + sync_pos, mp3_buf_used);
                        dump_hex_data(g_mp3_buf, 4, "MP3 header");
                    } else {
                        ESP_LOGE(TAG, "Invalid sync position: %zu >= %zu", sync_pos, mp3_buf_used);
                        mp3_buf_used = 0;
                        continue;
                    }
                }
                sync_found = true;
            } else {
                // Keep only last part of buffer for next search
                if (mp3_buf_used > 1024) {
                    size_t keep = 512;
                    if (keep < mp3_buf_used) {
                        memmove(g_mp3_buf, g_mp3_buf + (mp3_buf_used - keep), keep);
                        mp3_buf_used = keep;
                    }
                }
                continue;
            }
        }
        
        // Clear frame info
        memset(g_mp3_info, 0, sizeof(mp3dec_frame_info_t));
        
        // Decode frame
        int samples = mp3dec_decode_frame(g_mp3d, g_mp3_buf, (int)mp3_buf_used, g_pcm_buf, g_mp3_info);

        ESP_LOGI(TAG, "SAMPLES: %d, frame_bytes: %d", samples, g_mp3_info->frame_bytes);
        
        // Handle decode result
        if (samples > 0) {
            consecutive_errors = 0;
            frames_decoded++;
            
            if (frames_decoded % 50 == 0) {
                ESP_LOGI(TAG, "Frame %lu: %d samples, %dHz, %dch, %d bytes", 
                         frames_decoded, samples, g_mp3_info->hz, g_mp3_info->channels, g_mp3_info->frame_bytes);
            }
            
            // Validate parameters
            if (g_mp3_info->hz < 8000 || g_mp3_info->hz > 48000 || 
                g_mp3_info->channels < 1 || g_mp3_info->channels > 2) {
                ESP_LOGW(TAG, "Invalid audio params: %dHz, %dch", g_mp3_info->hz, g_mp3_info->channels);
                consecutive_errors++;
                goto handle_frame_removal;
            }
            
            // Update I2S sample rate
            if (first_frame || current_sample_rate != (uint32_t)g_mp3_info->hz) {
                esp_err_t ret = i2s_set_sample_rates(I2S_NUM, g_mp3_info->hz);
                if (ret == ESP_OK) {
                    current_sample_rate = g_mp3_info->hz;
                    ESP_LOGI(TAG, "Updated I2S to %dHz", g_mp3_info->hz);
                } else {
                    ESP_LOGW(TAG, "Failed to update I2S sample rate: %s", esp_err_to_name(ret));
                }
                first_frame = false;
            }
            
            // Convert mono to stereo if needed
            size_t pcm_bytes;
            if (g_mp3_info->channels == 1) {
                // Safety check for buffer overflow
                if (samples > PCM_BUFFER_SIZE / 2) {
                    ESP_LOGE(TAG, "Too many samples for stereo conversion: %d (max: %d)", samples, PCM_BUFFER_SIZE / 2);
                    consecutive_errors++;
                    goto handle_frame_removal;
                }
                
                // Convert in-place from end to beginning
                for (int i = samples - 1; i >= 0; i--) {
                    g_pcm_buf[i * 2] = g_pcm_buf[i];     // Left channel
                    g_pcm_buf[i * 2 + 1] = g_pcm_buf[i]; // Right channel
                }
                pcm_bytes = samples * 2 * sizeof(int16_t);
            } else {
                // Safety check for stereo
                if (samples * g_mp3_info->channels > PCM_BUFFER_SIZE) {
                    ESP_LOGE(TAG, "Too many samples for buffer: %d * %d (max: %d)", samples, g_mp3_info->channels, PCM_BUFFER_SIZE);
                    consecutive_errors++;
                    goto handle_frame_removal;
                }
                pcm_bytes = samples * g_mp3_info->channels * sizeof(int16_t);
            }
            
            // Write to I2S
            size_t bytes_written;
            esp_err_t ret = i2s_write(I2S_NUM, g_pcm_buf, pcm_bytes, &bytes_written, pdMS_TO_TICKS(100));
            
            if (ret != ESP_OK || bytes_written != pcm_bytes) {
                ESP_LOGW(TAG, "I2S write issue: %s, wrote %zu/%zu", esp_err_to_name(ret), bytes_written, pcm_bytes);
                // Don't count I2S write issues as consecutive errors since it's not a decode problem
            }
            
        } else if (samples == 0) {
            // Need more data or frame not complete
            ESP_LOGI(TAG, "No samples decoded, frame_bytes: %d", g_mp3_info->frame_bytes);
            
            if (g_mp3_info->frame_bytes > 0) {
                // Frame was processed but no samples (e.g., header frame)
                // Remove the frame and continue
                goto handle_frame_removal;
            } else {
                // Need more data - try to read more
                if (mp3_buf_used >= MP3_BUFFER_SIZE - STREAM_READ_SIZE) {
                    // Buffer is nearly full but still no valid frame
                    // This might indicate corrupted data, try to find new sync
                    ESP_LOGW(TAG, "Buffer full but no valid frame, searching for new sync");
                    sync_found = false;
                    mp3_buf_used = 0;
                }
                continue;
            }
        } else {
            // Decode error (samples < 0)
            ESP_LOGW(TAG, "Decode error: %d, frame_bytes: %d", samples, g_mp3_info->frame_bytes);
            consecutive_errors++;
            
            // If frame_bytes is valid, skip this frame
            if (g_mp3_info->frame_bytes > 0) {
                goto handle_frame_removal;
            } else {
                // Try to find next sync
                size_t sync_pos;
                if (mp3_buf_used > 1 && find_mp3_sync(g_mp3_buf + 1, mp3_buf_used - 1, &sync_pos)) {
                    sync_pos += 1; // Account for the +1 offset
                    if (sync_pos < mp3_buf_used) {
                        mp3_buf_used -= sync_pos;
                        memmove(g_mp3_buf, g_mp3_buf + sync_pos, mp3_buf_used);
                        ESP_LOGI(TAG, "Found new sync after decode error at position %zu", sync_pos);
                    } else {
                        sync_found = false;
                        mp3_buf_used = 0;
                    }
                } else {
                    sync_found = false;
                    mp3_buf_used = 0;
                    ESP_LOGW(TAG, "No sync found after decode error, resetting buffer");
                }
                continue;
            }
        }
        
handle_frame_removal:
        // Remove processed frame with safety checks
        if (g_mp3_info->frame_bytes > 0 && (size_t)g_mp3_info->frame_bytes <= mp3_buf_used) {
            // Additional safety check
            if ((size_t)g_mp3_info->frame_bytes > MP3_BUFFER_SIZE) {
                ESP_LOGE(TAG, "Frame bytes too large: %d > %d", g_mp3_info->frame_bytes, MP3_BUFFER_SIZE);
                sync_found = false;
                mp3_buf_used = 0;
            } else {
                mp3_buf_used -= g_mp3_info->frame_bytes;
                if (mp3_buf_used > 0) {
                    memmove(g_mp3_buf, g_mp3_buf + g_mp3_info->frame_bytes, mp3_buf_used);
                }
                ESP_LOGI(TAG, "Removed %d bytes from buffer, %zu bytes remaining", g_mp3_info->frame_bytes, mp3_buf_used);
            }
        } else if (g_mp3_info->frame_bytes > 0) {
            ESP_LOGW(TAG, "Invalid frame_bytes: %d (buffer: %zu)", g_mp3_info->frame_bytes, mp3_buf_used);
            sync_found = false;
            mp3_buf_used = 0;
        }
        
        // Prevent watchdog timeout
        if (frames_decoded % 10 == 0) {
            taskYIELD();
        }
    }
}

void output_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Output task started, free heap: %" PRIu32, esp_get_free_heap_size());

    // Verify stream buffer
    if (output_stream_buf == NULL) {
        ESP_LOGE(TAG, "Output stream buffer is NULL!");
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI(TAG, "Stream buffer available: %zu bytes", xStreamBufferBytesAvailable(output_stream_buf));

    // Allocate buffers
    if (!allocate_buffers()) {
        ESP_LOGE(TAG, "Failed to allocate buffers");
        vTaskDelete(NULL);
        return;
    }

    // Initialize I2S
    if (i2s_init() != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize I2S");
        free_buffers();
        vTaskDelete(NULL);
        return;
    }
    
    ESP_LOGI(TAG, "Free heap after init: %" PRIu32, esp_get_free_heap_size());

    // Start decoding
    decode_and_play_mp3_stream();

    // Cleanup
    ESP_LOGI(TAG, "Output task ending");
    i2s_driver_uninstall(I2S_NUM);
    free_buffers();
    vTaskDelete(NULL);
}