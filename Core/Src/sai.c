/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : SAI.c
  * Description        : This file provides code for the configuration
  *                      of the SAI instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "sai.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

SAI_HandleTypeDef hsai_BlockA1;
DMA_HandleTypeDef hdma_sai1_a;

/* SAI1 init function */
void MX_SAI1_Init(void)
{
  hsai_BlockA1.Instance = SAI1_Block_A;
  hsai_BlockA1.Init.AudioMode      = SAI_MODEMASTER_RX;
  hsai_BlockA1.Init.Synchro        = SAI_ASYNCHRONOUS;
  hsai_BlockA1.Init.OutputDrive    = SAI_OUTPUTDRIVE_DISABLE;
  hsai_BlockA1.Init.NoDivider      = SAI_MASTERDIVIDER_ENABLE;
  hsai_BlockA1.Init.FIFOThreshold  = SAI_FIFOTHRESHOLD_EMPTY;
  hsai_BlockA1.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_48K;
  hsai_BlockA1.Init.SynchroExt     = SAI_SYNCEXT_DISABLE;
  hsai_BlockA1.Init.MonoStereoMode = SAI_STEREOMODE;  // mic fills one channel
  hsai_BlockA1.Init.CompandingMode = SAI_NOCOMPANDING;

  if (HAL_SAI_InitProtocol(&hsai_BlockA1, 
                           SAI_I2S_STANDARD,
                           SAI_PROTOCOL_DATASIZE_24BIT, 
                           2) != HAL_OK)
  {
    Error_Handler();
  }
}

static uint32_t SAI1_client = 0;

void HAL_SAI_MspInit(SAI_HandleTypeDef* saiHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  if(saiHandle->Instance == SAI1_Block_A)
  {
    /* Configure SAI1 clock */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
    PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLL;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    if (SAI1_client == 0)
    {
       __HAL_RCC_SAI1_CLK_ENABLE();
    }
    SAI1_client++;

    /* SAI1_A_Block_A GPIO Configuration
       PE4     ------> SAI1_FS_A
       PE5     ------> SAI1_SCK_A
       PE6     ------> SAI1_SD_A
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF6_SAI1;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /* DMA setup for SAI1 */
    hdma_sai1_a.Instance = DMA1_Stream0;
    hdma_sai1_a.Init.Request = DMA_REQUEST_SAI1_A;
    hdma_sai1_a.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_sai1_a.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sai1_a.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sai1_a.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_sai1_a.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_sai1_a.Init.Mode = DMA_CIRCULAR;
    hdma_sai1_a.Init.Priority = DMA_PRIORITY_LOW;
    hdma_sai1_a.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_sai1_a) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(saiHandle, hdmarx, hdma_sai1_a);
    __HAL_LINKDMA(saiHandle, hdmatx, hdma_sai1_a);
  }
}

void HAL_SAI_MspDeInit(SAI_HandleTypeDef* saiHandle)
{
  if(saiHandle->Instance == SAI1_Block_A)
  {
    SAI1_client--;
    if (SAI1_client == 0)
    {
       __HAL_RCC_SAI1_CLK_DISABLE();
    }

    /* Deinit GPIO pins */
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6);

    /* Deinit DMA */
    HAL_DMA_DeInit(saiHandle->hdmarx);
    HAL_DMA_DeInit(saiHandle->hdmatx);
  }
}
