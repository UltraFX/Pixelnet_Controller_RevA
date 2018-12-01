/**
 * WS2812 Neopixel LED driver for STM32, Header
 * @Author: Nicolas Dammin, 2016
 */

#pragma once

#include "stm32f1xx_hal.h"

TIM_HandleTypeDef htim2;
DMA_HandleTypeDef hdma_tim2_ch3;

#define WS2812_NUM_LEDS_CH1		133

#define  WS2812_TIM_PRESCALE    0  // F_T3  = 72 MHz (13.88ns)
#define  WS2812_TIM_PERIODE   	89  // F_PWM = 800 kHz (1.25us)

#define  WS2812_LO_TIME        	29  // 29 * 13,9ns = 0.43us
#define  WS2812_HI_TIME        	58  // 58 * 13.9ns = 0.81us

//--------------------------------------------------------------
// RGB LED Farbdefinition (3 x 8bit)
//--------------------------------------------------------------
typedef struct {
  uint8_t red;    // 0...255 (als PWM-Wert)
  uint8_t green;  // 0...255 (als PWM-Wert)
  uint8_t blue;   // 0...255 (als PWM-Wert)
}WS2812_RGB_t;


//--------------------------------------------------------------
// HSV LED Farbdefinition
//--------------------------------------------------------------
typedef struct {
  uint16_t h;     // 0...359 (in Grad, 0=R, 120=G, 240=B)
  uint8_t s;      // 0...100 (in Prozent)
  uint8_t v;      // 0...100 (in Prozent)
}WS2812_HSV_t;

WS2812_RGB_t WS2812_LED_BUF_CH1[WS2812_NUM_LEDS_CH1];

#define WS2812_BUFLEN	(WS2812_NUM_LEDS_CH1+2)*24+5

//Interrupt Callback
void DMA_Callback(void);

//Library Interface
void WS2812_Refresh(void);
void WS2812_Clear(void);
void WS2812_RGB2HSV(WS2812_HSV_t hsv_col, WS2812_RGB_t *rgb_col);
void WS2812_One_RGB(uint32_t nr, WS2812_RGB_t rgb_col, uint8_t refresh);
void WS2812_All_RGB(WS2812_RGB_t rgb_col, uint8_t refresh);
void WS2812_One_HSV(uint32_t nr, WS2812_HSV_t hsv_col, uint8_t refresh);
void WS2812_All_HSV(WS2812_HSV_t hsv_col, uint8_t refresh);
void WS2812_Shift_Left(uint8_t refresh);
void WS2812_Shift_Right(uint8_t refresh);
void WS2812_Rotate_Left(uint8_t refresh);
void WS2812_Rotate_Right(uint8_t refresh);
void WS2812_Set_Data(uint8_t *pbyData, uint16_t wStartAddress);


