#ifndef I2S_AUDIO_H
#define I2S_AUDIO_H

#include <stdint.h>
#include <stdbool.h>  // <--- needed for 'bool'
#include <stddef.h>

#define AUDIO_BUFFER_SIZE 1024

void I2S_Audio_Init(void);
void I2S_Audio_Process(uint32_t *samples, uint32_t num_samples);

// Circular buffer interface
extern volatile bool new_samples_ready;
uint32_t I2S_GetSample(void);

#endif // I2S_AUDIO_H
