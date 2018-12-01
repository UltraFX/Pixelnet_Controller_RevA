/*
 * TLC5947_Lib.c
 *
 *  Created on: 29.09.2018
 *      Author: ndamm
 */

#include <stdint.h>
#include <string.h>
#include "TLC5947_Lib.h"
#include "stm32f1xx_hal.h"
#include "spi.h"

uint8_t byPWMData[NUM_BUF] = {0};
uint16_t wChannel[NUM_CHANNELS] = {0};

void TLC5947_set_channel(uint8_t byChannel, uint16_t wValue)
{
	if((byChannel > NUM_CHANNELS) || byChannel == 0)	/* wrong channel number (bigger than available channels) */
	{
		return;
	}

	wChannel[byChannel-1] = wValue;
}

void TLC5947_SetData(uint8_t *pbyData, uint16_t wStartByte)
{
	uint8_t byI;

//	memcpy(wChannel, &pbyData[wStartByte-1], sizeof(uint16_t)NUM_CHANNELS)

	for(byI = 0; byI < NUM_CHANNELS; byI++)
	{
		wChannel[byI] = ((uint16_t)pbyData[wStartByte-1+byI])*16;
	}
}

//  byTest[35] = 0xff;
//  byTest[34] = 0xf0;
//  byTest[31] = 0x0f;
//  byTest[30] = 0xff;

void TLC5947_Refresh(void)
{
	uint8_t byChan;
	uint8_t byEven = 0;
	uint8_t byBuf;

	byBuf = NUM_BUF - 1;
	for(byChan = 0; byChan < NUM_CHANNELS; byChan++)
	{
		if(byEven == 0)
		{
			byPWMData[byBuf] = wChannel[byChan] & 0xFF;
			byBuf--;
			byPWMData[byBuf] &= 0xF0;
			byPWMData[byBuf] |= (wChannel[byChan] >> 8)&0x0F;
		}
		else
		{
			byPWMData[byBuf] &= 0x0F;
			byPWMData[byBuf] |= (wChannel[byChan] << 4)&0xF0;
			byBuf--;
			byPWMData[byBuf] = (wChannel[byChan] >> 4) & 0xFF;
			byBuf--;
		}
		byEven ^= 1;
	}

	HAL_SPI_Transmit(&hspi2, byPWMData, NUM_BUF, 0xffffffff);

	HAL_GPIO_WritePin(LATCH_GPIO_Port, LATCH_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(LATCH_GPIO_Port, LATCH_Pin, GPIO_PIN_RESET);
}
