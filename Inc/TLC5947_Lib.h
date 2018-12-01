/*
 * TLC5947_Lib.h
 *
 *  Created on: 29.09.2018
 *      Author: ndamm
 */

#ifndef TLC5947_LIB_H_
#define TLC5947_LIB_H_

#define NUM_CHANNELS	12

#define NUM_BUF	(NUM_CHANNELS*12/8)+1

void TLC5947_set_channel(uint8_t byChannel, uint16_t wValue);
void TLC5947_Refresh(void);
void TLC5947_SetData(uint8_t *pbyData, uint16_t wStartByte);

#endif /* TLC5947_LIB_H_ */
