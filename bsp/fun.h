#ifndef	_FUN_H
#define _FUN_H

#include "main.h"

double ave_vol(uint16_t *adc_data);
void Compute(void);
void Result_Dis(void);
void ChangeFreq(void);
void stringToHexWithSuffix(int n, double value);
void Debug(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
extern uint8_t rec_data;
extern int view; 
#endif