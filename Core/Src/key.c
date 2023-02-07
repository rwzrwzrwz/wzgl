#include "key.h"


uint8_t JzKey()   //矩阵按键程序
{
	uint8_t KeyValue=0;
	uint8_t a;
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : PF4 PF5 PF6 PF7 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_SET);//矩阵按键低4位低电平，高四位为高电平,测试行

	if((GPIOF->IDR&0xff)!=0x0f)
	{
		HAL_Delay(10);				//消抖
		if((GPIOF->IDR&0xff)!=0x0f)		
		{
				switch(GPIOF->IDR&0xff)
				{
					case(0x1f): KeyValue=1;break;
					case(0x2f): KeyValue=2;break;
					case(0x4f): KeyValue=3;break;
					case(0x8f): KeyValue=4;break;
				}

				GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
				GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
				GPIO_InitStruct.Pull = GPIO_NOPULL;
				GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
				HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

				/*Configure GPIO pins : PF0 PF1 PF2 PF3 */
				GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
				GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
				GPIO_InitStruct.Pull = GPIO_PULLDOWN;
				HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
				
				
				HAL_GPIO_WritePin(GPIOF, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_SET);//与上相反，测试列			
				switch(GPIOF->IDR&0xff)
				{
					case(0xf1): KeyValue=KeyValue;break;
					case(0xf2): KeyValue=KeyValue+4;break;
					case(0xf4): KeyValue=KeyValue+8;break;
					case(0xf8): KeyValue=KeyValue+12;break;					
				}
				while((a<50)&&((GPIOF->IDR&0xff)!=0xf0))	 //检测按键松手检测,0.5s后还不松手也触发
				{
					HAL_Delay(10);
					a++;
				}
				
			
		}
	}
	return KeyValue;
}
