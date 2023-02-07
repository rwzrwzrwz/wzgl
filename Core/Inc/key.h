#ifndef __KEY_H__
#define __KEY_H__
#include "main.h"
#define GPIO_WriteLow(GPIOx,a)    GPIOx->BSRR=(((uint32_t)(uint8_t)~(a))<<16)|((uint32_t)(uint8_t)(a))
#define GPIO_WriteHigh(GPIOx,a)    GPIOx->BSRR=(((uint8_t)(uint8_t)~(a))<<24)|(((uint32_t)(uint8_t)(a))<<8)



uint8_t JzKey(void);

#endif
