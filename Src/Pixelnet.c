/*
 * Pixelnet.c
 *
 *  Created on: 29.09.2018
 *      Author: ndamm
 */

#include "stm32f1xx_hal.h"
#include "usart.h"
#include "WS2812_Lib.h"
#include "TLC5947_Lib.h"
#include "Pixelnet.h"


uint8_t byPixelData[4096] = {0};
volatile uint8_t byRdy;
uint32_t dwCount = 0;

volatile uint8_t byBreak = 0;

void Pix_UART_CB(void)
{
	static uint16_t wCount = 0;
	static uint8_t byStart = 0;
	uint8_t byData;

	byData = (uint8_t)USART1->DR;

#ifdef PIXELNET_MODE
	if(byData == 170)
	{
		dwCount++;
		wCount = 0;
		byRdy = 1;
	}
	else
	{
		byPixelData[wCount++] = byData;
	}
#endif

#ifdef DMX_MODE
	if(byBreak)
	{
		byStart = 0;
		wCount = 0;
		byBreak = 0;
		byRdy = 1;
	}
	else
	{
		if(byStart == 0)
		{
			byStart = 1;
		}
		else
		{
			if(wCount < 4095)
			{
				byPixelData[wCount++] = byData;
			}
		}
	}
#endif
}

void Pix_UART_Break(void)
{
	byBreak = 1;
}

void Pix_Handler(void)
{
	if(byRdy == 1)
	{
		byRdy = 0;
		TLC5947_SetData(byPixelData, 400);

		TLC5947_Refresh();
		WS2812_Set_Data(byPixelData, 1);
	}
}
