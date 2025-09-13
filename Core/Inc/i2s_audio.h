#ifndef __I2S_AUDIO_H
#define __I2S_AUDIO_H
#ifndef AUDIO_BUFFER_SIZE
#define AUDIO_BUFFER_SIZE 1024   // number of 32-bit words
#endif

#include "main.h"
#include <stdint.h>

void I2S_Audio_Init(void);

// Called when half/full DMA buffer is ready
void I2S_Audio_Process(uint32_t *samples, uint32_t num_samples);

#endif

