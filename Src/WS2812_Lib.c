/**
 * WS2812 Neopixel LED driver for STM32

 * Based upon the WS2812 Library for STM32F4 from Uwe Becker, http://mikrocontroller.bplaced.net/wordpress/?page_id=3665
 *
 * @Author: Nicolas Dammin, 2016
 */
#include <string.h>
#include "stm32f1xx_hal.h"
#include "WS2812_Lib.h"

uint16_t WS2812_TIM_BUF[WS2812_BUFLEN];

WS2812_RGB_t waOutBuf[WS2812_NUM_LEDS_CH1];

uint8_t dma_ready = 1;

void DMA_Callback(void) {
	dma_ready = 1;
}

void WS2812_Align_LEDs(void)
{
	uint8_t byRow = 0;
	uint8_t byCol = 0;
	uint8_t byLED = 0;

	for(byRow = 0; byRow < 6; byRow++)
	{
		for(byCol = 0; byCol < 22; byCol++)
		{
			if(byRow%2==0)
			{
				waOutBuf[byLED] = WS2812_LED_BUF_CH1[byLED];
			}
			else
			{
				waOutBuf[byLED] = WS2812_LED_BUF_CH1[(22*(byRow+1))-byCol-1];
			}
			byLED++;
		}
	}
}

/**
 * Internal function, calculates the HI or LO values for the 800 kHz WS2812 signal and puts them into a buffer for the Timer-DMA
 *
 */
void calcBuf(void)
{
  uint32_t n;
  uint32_t pos;
  WS2812_RGB_t led;

  pos=0;

  WS2812_Align_LEDs();

  WS2812_TIM_BUF[pos++]=0;
  WS2812_TIM_BUF[pos++]=0;
  WS2812_TIM_BUF[pos++]=0;
  WS2812_TIM_BUF[pos++]=0;
  WS2812_TIM_BUF[pos++]=0;

  // set timings for all LEDs
  for(n=0;n<WS2812_NUM_LEDS_CH1;n++) {
    led=waOutBuf[n];//WS2812_LED_BUF_CH1[n];

    // Col:Green , Bit:7..0
    WS2812_TIM_BUF[pos++]=((led.green&0x80) != 0)?WS2812_HI_TIME:WS2812_LO_TIME;
    WS2812_TIM_BUF[pos++]=((led.green&0x40) != 0)?WS2812_HI_TIME:WS2812_LO_TIME;
    WS2812_TIM_BUF[pos++]=((led.green&0x20) != 0)?WS2812_HI_TIME:WS2812_LO_TIME;
    WS2812_TIM_BUF[pos++]=((led.green&0x10) != 0)?WS2812_HI_TIME:WS2812_LO_TIME;
    WS2812_TIM_BUF[pos++]=((led.green&0x08) != 0)?WS2812_HI_TIME:WS2812_LO_TIME;
    WS2812_TIM_BUF[pos++]=((led.green&0x04) != 0)?WS2812_HI_TIME:WS2812_LO_TIME;
    WS2812_TIM_BUF[pos++]=((led.green&0x02) != 0)?WS2812_HI_TIME:WS2812_LO_TIME;
    WS2812_TIM_BUF[pos++]=((led.green&0x01) != 0)?WS2812_HI_TIME:WS2812_LO_TIME;

    // Col:Red , Bit:7..0
    WS2812_TIM_BUF[pos++]=((led.red&0x80) != 0)?WS2812_HI_TIME:WS2812_LO_TIME;
	WS2812_TIM_BUF[pos++]=((led.red&0x40) != 0)?WS2812_HI_TIME:WS2812_LO_TIME;
	WS2812_TIM_BUF[pos++]=((led.red&0x20) != 0)?WS2812_HI_TIME:WS2812_LO_TIME;
	WS2812_TIM_BUF[pos++]=((led.red&0x10) != 0)?WS2812_HI_TIME:WS2812_LO_TIME;
	WS2812_TIM_BUF[pos++]=((led.red&0x08) != 0)?WS2812_HI_TIME:WS2812_LO_TIME;
	WS2812_TIM_BUF[pos++]=((led.red&0x04) != 0)?WS2812_HI_TIME:WS2812_LO_TIME;
	WS2812_TIM_BUF[pos++]=((led.red&0x02) != 0)?WS2812_HI_TIME:WS2812_LO_TIME;
	WS2812_TIM_BUF[pos++]=((led.red&0x01) != 0)?WS2812_HI_TIME:WS2812_LO_TIME;

	// Col:Blue , Bit:7..0
	WS2812_TIM_BUF[pos++]=((led.blue&0x80) != 0)?WS2812_HI_TIME:WS2812_LO_TIME;
	WS2812_TIM_BUF[pos++]=((led.blue&0x40) != 0)?WS2812_HI_TIME:WS2812_LO_TIME;
	WS2812_TIM_BUF[pos++]=((led.blue&0x20) != 0)?WS2812_HI_TIME:WS2812_LO_TIME;
	WS2812_TIM_BUF[pos++]=((led.blue&0x10) != 0)?WS2812_HI_TIME:WS2812_LO_TIME;
	WS2812_TIM_BUF[pos++]=((led.blue&0x08) != 0)?WS2812_HI_TIME:WS2812_LO_TIME;
	WS2812_TIM_BUF[pos++]=((led.blue&0x04) != 0)?WS2812_HI_TIME:WS2812_LO_TIME;
	WS2812_TIM_BUF[pos++]=((led.blue&0x02) != 0)?WS2812_HI_TIME:WS2812_LO_TIME;
	WS2812_TIM_BUF[pos++]=((led.blue&0x01) != 0)?WS2812_HI_TIME:WS2812_LO_TIME;
  }

  // short pause after all LEDs have been updated
  for(n=0;n<48;n++) {
    WS2812_TIM_BUF[pos++]=0;
  }
}

/**
 * Internal function; start DMA transfer
 */
void startDMA(void) {
	//uint8_t test[8] = {10};
	dma_ready = 0;
	HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_3, (uint32_t *)WS2812_TIM_BUF, WS2812_BUFLEN);
}

void WS2812_Refresh(void) {

	while(!dma_ready);
	calcBuf();
	startDMA();
}

/**
 * Set all LEDs to 0 (off) and update
 */
void WS2812_Clear(void) {
	uint16_t num;

	for(num = 0; num < WS2812_NUM_LEDS_CH1; num++) {
		WS2812_LED_BUF_CH1[num] = (WS2812_RGB_t){0,0,0};
	}

	WS2812_Refresh();
}

/**
 * Convert HSV-Value to RGB Value for WS2812 LEDs
 * (from www.ulrichradig.de)
 */
void WS2812_RGB2HSV(WS2812_HSV_t hsv_col, WS2812_RGB_t *rgb_col)
{
  uint8_t diff;

  // Grenzwerte
  if(hsv_col.h>359) hsv_col.h=359;
  if(hsv_col.s>100) hsv_col.s=100;
  if(hsv_col.v>100) hsv_col.v=100;

  if(hsv_col.h < 61) {
    rgb_col->red = 255;
    rgb_col->green = (425 * hsv_col.h) / 100;
    rgb_col->blue = 0;
  }else if(hsv_col.h < 121){
    rgb_col->red = 255 - ((425 * (hsv_col.h-60))/100);
    rgb_col->green = 255;
    rgb_col->blue = 0;
  }else if(hsv_col.h < 181){
    rgb_col->red = 0;
    rgb_col->green = 255;
    rgb_col->blue = (425 * (hsv_col.h-120))/100;
  }else if(hsv_col.h < 241){
    rgb_col->red = 0;
    rgb_col->green = 255 - ((425 * (hsv_col.h-180))/100);
    rgb_col->blue = 255;
  }else if(hsv_col.h < 301){
    rgb_col->red = (425 * (hsv_col.h-240))/100;
    rgb_col->green = 0;
    rgb_col->blue = 255;
  }else {
    rgb_col->red = 255;
    rgb_col->green = 0;
    rgb_col->blue = 255 - ((425 * (hsv_col.h-300))/100);
  }

  hsv_col.s = 100 - hsv_col.s;
  diff = ((255 - rgb_col->red) * hsv_col.s)/100;
  rgb_col->red = rgb_col->red + diff;
  diff = ((255 - rgb_col->green) * hsv_col.s)/100;
  rgb_col->green = rgb_col->green + diff;
  diff = ((255 - rgb_col->blue) * hsv_col.s)/100;
  rgb_col->blue = rgb_col->blue + diff;

  rgb_col->red = (rgb_col->red * hsv_col.v)/100;
  rgb_col->green = (rgb_col->green * hsv_col.v)/100;
  rgb_col->blue = (rgb_col->blue * hsv_col.v)/100;
}

void WS2812_Set_Data(uint8_t *pbyData, uint16_t wStartAddress)
{
	uint8_t wPos;

//	for(wPos = 0; wPos < WS2812_NUM_LEDS_CH1; wPos++)
//	{
//		WS2812_LED_BUF_CH1[wPos] = pbyData[wStartAddress+wPos];
//	}
	memcpy(WS2812_LED_BUF_CH1, &pbyData[wStartAddress-1], sizeof(uint8_t)*WS2812_NUM_LEDS_CH1*3);
	WS2812_Refresh();
}

/**
 * Set one LED (R, G, B values). If refresh == 1, update LEDs, otherwise just update buffer (if several function calls are to be done before refresh)
 */
void WS2812_One_RGB(uint32_t nr, WS2812_RGB_t rgb_col, uint8_t refresh)
{
  if(nr<WS2812_NUM_LEDS_CH1) {
	  WS2812_LED_BUF_CH1[nr]=rgb_col;

    if(refresh==1) WS2812_Refresh();
  }
}

/**
 * Set all LEDs (R, G, B values). If refresh == 1, update LEDs, otherwise just update buffer (if several function calls are to be done before refresh)
 */
void WS2812_All_RGB(WS2812_RGB_t rgb_col, uint8_t refresh)
{
  uint32_t n;

  for(n=0;n<WS2812_NUM_LEDS_CH1;n++) {
	  WS2812_LED_BUF_CH1[n]=rgb_col;
  }
  if(refresh==1) WS2812_Refresh();
}

/**
 * Set one LED (H, S, V values). If refresh == 1, update LEDs, otherwise just update buffer (if several function calls are to be done before refresh)
 */
void WS2812_One_HSV(uint32_t nr, WS2812_HSV_t hsv_col, uint8_t refresh)
{
  WS2812_RGB_t rgb_col;

  if(nr<WS2812_NUM_LEDS_CH1) {
    // convert to RGB
    WS2812_RGB2HSV(hsv_col, &rgb_col);
    WS2812_LED_BUF_CH1[nr]=rgb_col;

    if(refresh==1) WS2812_Refresh();
  }
}

/**
 * Set all LEDs (H, S, V values). If refresh == 1, update LEDs, otherwise just update buffer (if several function calls are to be done before refresh)
 */
void WS2812_All_HSV(WS2812_HSV_t hsv_col, uint8_t refresh)
{
  uint32_t n;
  WS2812_RGB_t rgb_col;

  // convert to RGB
  WS2812_RGB2HSV(hsv_col, &rgb_col);
  for(n=0;n<WS2812_NUM_LEDS_CH1;n++) {
	  WS2812_LED_BUF_CH1[n]=rgb_col;
  }
  if(refresh==1) WS2812_Refresh();
}

/**
 * Shift all LED values one to the left. Last one will be turned off
 */
void WS2812_Shift_Left(uint8_t refresh)
{
  uint32_t n;

  if(WS2812_NUM_LEDS_CH1>1) {
    for(n=1;n<WS2812_NUM_LEDS_CH1;n++) {
    	WS2812_LED_BUF_CH1[n-1]=WS2812_LED_BUF_CH1[n];
    }
    WS2812_LED_BUF_CH1[n-1]=(WS2812_RGB_t){0,0,0};

    if(refresh==1) WS2812_Refresh();
  }
}

/**
 * Shift all LED values one to the right. First one will be turned off
 */
void WS2812_Shift_Right(uint8_t refresh)
{
  uint32_t n;

  if(WS2812_NUM_LEDS_CH1>1) {
    for(n=WS2812_NUM_LEDS_CH1-1;n>0;n--) {
    	WS2812_LED_BUF_CH1[n]=WS2812_LED_BUF_CH1[n-1];
    }
    WS2812_LED_BUF_CH1[n]=(WS2812_RGB_t){0,0,0};

    if(refresh==1) WS2812_Refresh();
  }
}

/**
 * Shift all LED values one to the left. Last LED value will be the previous first value
 */
void WS2812_Rotate_Left(uint8_t refresh)
{
  uint32_t n;
  WS2812_RGB_t d;

  if(WS2812_NUM_LEDS_CH1>1) {
    d=WS2812_LED_BUF_CH1[0];
    for(n=1;n<WS2812_NUM_LEDS_CH1;n++) {
    	WS2812_LED_BUF_CH1[n-1]=WS2812_LED_BUF_CH1[n];
    }
    WS2812_LED_BUF_CH1[n-1]=d;

    if(refresh==1) WS2812_Refresh();
  }
}

/**
 * Shift all LED values one to the right. First LED value will be the previous last value
 */
void WS2812_Rotate_Right(uint8_t refresh)
{
  uint32_t n;
  WS2812_RGB_t d;

  if(WS2812_NUM_LEDS_CH1>1) {
    d=WS2812_LED_BUF_CH1[WS2812_NUM_LEDS_CH1-1];
    for(n=WS2812_NUM_LEDS_CH1-1;n>0;n--) {
    	WS2812_LED_BUF_CH1[n]=WS2812_LED_BUF_CH1[n-1];
    }
    WS2812_LED_BUF_CH1[n]=d;

    if(refresh==1) WS2812_Refresh();
  }
}

