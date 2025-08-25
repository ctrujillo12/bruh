#include "i2s_audio.h"
#include "stm32h7xx_hal_i2s.h"

extern I2S_HandleTypeDef hi2s2;

// Example 16-bit stereo test tone buffer (dummy data)
uint16_t i2s_tx_buf[32] = {
    0x0000, 0x7FFF, 0xFFFF, 0x8000, // square-ish wave pattern
    0x0000, 0x7FFF, 0xFFFF, 0x8000,
    0x0000, 0x7FFF, 0xFFFF, 0x8000,
    0x0000, 0x7FFF, 0xFFFF, 0x8000
};

void MX_I2S2_Init(void)
{
    hi2s2.Instance = SPI2;   // SPI2/I2S2 block
    hi2s2.Init.Mode = I2S_MODE_MASTER_TX;
    hi2s2.Init.Standard = I2S_STANDARD_PHILIPS;
    hi2s2.Init.DataFormat = I2S_DATAFORMAT_16B;
    hi2s2.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
    hi2s2.Init.AudioFreq = I2S_AUDIOFREQ_44K;
    hi2s2.Init.CPOL = I2S_CPOL_LOW;
    // hi2s2.Init.ClockSource = I2S_CLOCK_PLL;
    // hi2s2.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
    if (HAL_I2S_Init(&hi2s2) != HAL_OK)
    {
        Error_Handler();
    }
}

void i2s_audio_start(void)
{
    // Kick off DMA transfer (looping static buffer)
    HAL_I2S_Transmit_DMA(&hi2s2, i2s_tx_buf, sizeof(i2s_tx_buf)/sizeof(uint16_t));
}
