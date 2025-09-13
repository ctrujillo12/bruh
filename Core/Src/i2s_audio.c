// Core/Src/i2s_audio.c
#include "i2s_audio.h"
#include "sai.h"
#include "usart.h"
#include "stm32h7xx_hal.h"
#include <stdbool.h>   // for bool

// -------------------------- Configuration ----------------------------
#define AUDIO_BUFFER_SIZE      1024   // DMA buffer size (32-bit samples)
#define CIRCULAR_BUFFER_SIZE   2048   // Circular buffer size

// -------------------------- Buffers ----------------------------
// DMA double-buffered reception
static uint32_t i2s_rx_buffer[AUDIO_BUFFER_SIZE];

// Circular buffer for processed audio
static uint32_t circular_buffer[CIRCULAR_BUFFER_SIZE];
static volatile uint32_t circ_write_idx = 0;
static volatile uint32_t circ_sample_count = 0;

// -------------------------- Flags ----------------------------
volatile bool new_samples_ready = false; // signal main loop
static uint32_t latest_sample = 0;

// -------------------------- Initialization ----------------------------
void I2S_Audio_Init(void)
{
    MX_SAI1_Init(); // CubeMX-generated init for SAI1

    // Start DMA reception
    if (HAL_SAI_Receive_DMA(&hsai_BlockA1,
                            (uint8_t *)i2s_rx_buffer,
                            AUDIO_BUFFER_SIZE) != HAL_OK)
    {
        Error_Handler();
    }
}

// -------------------------- DMA Callbacks ----------------------------
void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
    if (hsai->Instance == SAI1_Block_A)
    {
        for (uint32_t i = 0; i < AUDIO_BUFFER_SIZE / 2; i++)
        {
            circular_buffer[circ_write_idx] = i2s_rx_buffer[i];
            circ_write_idx = (circ_write_idx + 1) % CIRCULAR_BUFFER_SIZE;
            if (circ_sample_count < CIRCULAR_BUFFER_SIZE)
            {
                circ_sample_count++;
            }
        }
        new_samples_ready = true;
    }
}

void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
{
    if (hsai->Instance == SAI1_Block_A)
    {
        for (uint32_t i = AUDIO_BUFFER_SIZE / 2; i < AUDIO_BUFFER_SIZE; i++)
        {
            circular_buffer[circ_write_idx] = i2s_rx_buffer[i];
            circ_write_idx = (circ_write_idx + 1) % CIRCULAR_BUFFER_SIZE;
            if (circ_sample_count < CIRCULAR_BUFFER_SIZE)
            {
                circ_sample_count++;
            }
            BSP_LED_Toggle(LED_YELLOW);

        }
        new_samples_ready = true;
    }
}

// -------------------------- Processing ----------------------------
// Weak function can be overridden if desired
__weak void I2S_Audio_Process(uint32_t *samples, uint32_t num_samples)
{
    latest_sample = samples[0];  // store first sample for main loop
    new_samples_ready = true;     // signal main loop
}

// -------------------------- Main Loop Helper ----------------------------
uint32_t I2S_GetSample(void)
{
    static uint32_t read_idx = 0;
    uint32_t sample = 0;

    if (circ_sample_count > 0)
    {
        sample = circular_buffer[read_idx];
        read_idx = (read_idx + 1) % CIRCULAR_BUFFER_SIZE;

        __disable_irq();
        circ_sample_count--;
        __enable_irq();
    }

    return sample;
}
