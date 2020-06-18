/*
 * NFC_SPI.c
 *
 *  Created on: May 24, 2020
 *      Author: hanes
 */

#include <NFC_SPI.h>

#define true	(1)
#define false	(0)

SPI_HandleTypeDef hspi;

uint8_t NFC_SPI_Init(void)
{
	hspi.Instance = SPI2;
	hspi.Init.Mode = SPI_MODE_MASTER;
	hspi.Init.Direction = SPI_DIRECTION_2LINES;
	hspi.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi.Init.NSS = SPI_NSS_SOFT;
	hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
	hspi.Init.FirstBit = SPI_FIRSTBIT_LSB;
	hspi.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi.Init.CRCPolynomial = 7;
	hspi.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	hspi.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;

	if (HAL_SPI_Init(&hspi) != HAL_OK)
	{
		return false;
	}

	return true;
}

void HAL_SPI_MspInit(SPI_HandleTypeDef *spiHandle)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	if (spiHandle->Instance == SPI2)
	{
		/* SPI2 clock enable */
	    __HAL_RCC_SPI2_CLK_ENABLE();

	    __HAL_RCC_GPIOA_CLK_ENABLE();
	    __HAL_RCC_GPIOH_CLK_ENABLE();
	    __HAL_RCC_GPIOB_CLK_ENABLE();

	    /**
	    PA12     ------> SPI2_SCK
	    PA11     ------> SPI2_NSS
	    PB14     ------> SPI2_MISO
	    PB15     ------> SPI2_MOSI
	    */

	    /* Configuration pin of CLOCK SPI  of NFC*/
	    GPIO_InitStruct.Pin = SPI_SCK_Pin;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
	    HAL_GPIO_Init(SPI_SCK_GPIO_Port, &GPIO_InitStruct);

	    /* Configuration pin of CS SPI  of NFC*/
	    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);

	    GPIO_InitStruct.Pin = SPI_CS_Pin;
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    HAL_GPIO_Init(SPI_CS_GPIO_Port, &GPIO_InitStruct);

	    /* Configuration pin of MISO SPI  of NFC*/
	    GPIO_InitStruct.Pin = SPI_MISO_Pin;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
	    HAL_GPIO_Init(SPI_MISO_GPIO_Port, &GPIO_InitStruct);

	    /* Configuration pin of MOSI SPI  of NFC*/
	    GPIO_InitStruct.Pin = SPI_MOSI_Pin;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
	    HAL_GPIO_Init(SPI_MOSI_GPIO_Port, &GPIO_InitStruct);

	    /* Configuration of pin IRQ of NFC */
	    GPIO_InitStruct.Pin = NFC_IRQ_Pin;
	    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    HAL_GPIO_Init(NFC_IRQ_GPIO_Port, &GPIO_InitStruct);
	}
}

uint8_t NFC_SPI_GetByte(void)
{
	uint8_t byte = 0x00;

	while (HAL_SPI_GetState(&hspi) != HAL_SPI_STATE_READY); 			//Is possble receive?
	HAL_SPI_Receive(&hspi, &byte, 1, SPI_NFC_TIMEOUT_RECEPTION);

	return byte;
}

void NFC_SPI_SendByte(const uint8_t byte)
{
	while (HAL_SPI_GetState(&hspi) != HAL_SPI_STATE_READY); 					//Is possble transmit?
	HAL_SPI_Transmit(&hspi, (uint8_t *)&byte, 1, SPI_NFC_TIMEOUT_TRANSMISSION);	// send 8 bits of data
	while (HAL_SPI_GetState(&hspi) == HAL_SPI_STATE_BUSY);						//Transmission ready?
}

void NFC_SPI_SetSelect(const uint8_t state)
{
	if (state)
	{
		HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
	}
}

uint8_t NFC_SPI_GetIRQ(void)
{
	if ( HAL_GPIO_ReadPin(NFC_IRQ_GPIO_Port, NFC_IRQ_Pin) == GPIO_PIN_SET )
	{
		return false;
	}

	return true;
}
