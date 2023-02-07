/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "main.h"
#include "dma.h"
#include "fatfs.h"
#include "sdio.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "key.h"
#include "lcd.h"
#include "rfid.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//格式化文件系统时使用
BYTE work[_MAX_SS];
u8 wBuff[512];
char id_number[200][10];
char filename_id[]="成员信息.csv";
char filename_yy[]="运营组物资.csv";
char filename_dk[]="电控组物资.csv";
char filename_sj[]="视觉组物资.csv";
char filename_jx[]="机械组物资.csv";
char filename_fk[]="飞控组物资.csv";

struct WZ Yy_Wz[500];
struct WZ Fk_Wz[500];
struct WZ Sj_Wz[500];
struct WZ Dk_Wz[500];
struct WZ Jx_Wz[500];

char card_id[10]="8203191207";

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */


  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_FSMC_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_SDIO_SD_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
 	LCD_Init();           			//初始化LCD FSMC接口
	POINT_COLOR=WHITE;     			//画笔颜色：白色
	LCD_Clear(BLACK);
	
	LCD_ShowLine(1,"loading......");
	
	//挂载文件系统
	retSD=f_mount(&SDFatFS,SDPath,1);
	switch (retSD)
  {
  	case FR_NO_FILESYSTEM:
			{
				retSD=f_mkfs(SDPath,FM_FAT32,512,work,sizeof work);
				if(FR_OK!=retSD)
					LCD_ShowLine(2,"No File System");
			}
  		break;
  	case FR_OK:LCD_ShowLine(2,"Sd Card Load Ok");
  		break;
  	default:LCD_ShowLine(2,"Sd Card Error");
  		break;
  }
	//加载ID信息
	Load_Id();
	//加载物资信息
	Load_Data(Yy_Wz,filename_yy);
	Load_Data(Fk_Wz,filename_fk);
	Load_Data(Sj_Wz,filename_sj);
	Load_Data(Dk_Wz,filename_dk);
	Load_Data(Jx_Wz,filename_jx);
	
	sprintf((char *)wBuff,"%.10s",id_number[1]);
	LCD_ShowLine(5,wBuff);
	
	sprintf((char *)wBuff,"%.8s",Yy_Wz[1].id);
	LCD_ShowLine(6,wBuff);
	
	HAL_Delay(1000);
	
	
	/*
	sprintf((char *)wBuff,"%s","hello world\n");
	retSD=f_write(&SDFile,wBuff,strlen((const char *)wBuff),&numRW);
	if(FR_OK!=retSD)
			LCD_ShowString(30,80,210,24,12,"success write");
	
	f_close(&SDFile);
	
*/
	
	/*
	//处理数据
	while(rBuff[point]!=0x0a)point++;
	point++;
	line++;
	
	while(rBuff[point]!=0x00)
	{
		//读取id
		for(i=0;i<8;i++)
		{
			wz[line].id[i]=rBuff[point];
			point++;
		}
		point++;//跳过逗号
		//读取name
		i=0;
		while(rBuff[point]!=0x0a)
		{
			if(rBuff[point]!=0x0d)
				wz[line].name[i]=rBuff[point];
			point++;
		}
		point++;
		line++;
	}
*/
	
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		main_page();
		
		/*
		status = ReadId(card_id);  //读卡号
		if(!status)
			LCD_ShowLine(7,card_id);
		
		HAL_Delay(200);
		status = ReadDataFromBlock(card_blockdata,2);
		HAL_Delay(200);
		*/
		/*
		uint8_t KeyValue=0;
		KeyValue=JzKey();
		if(KeyValue)
		{
			LCD_Clear(BLACK);
			LCD_ShowxNum(30,40,KeyValue,2,24,1);
		}
		*/
/*
		u8 message[9];
		u8 i;
		for(i=0;i<9;i++)
		{
			message[i]=getchar();
		}

		LCD_Clear(BLACK);
		LCD_ShowString(30,40,210,24,12,message);
		
*/
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_Delay_us(uint32_t us)
{       
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000000);
    HAL_Delay(us-1);
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
}

void Load_Id()
{
	char rBuff[1024];//读取数据暂存
	u16 point=0;
	u16 line=0;
	u8 i;
	//打开文件
	retSD=f_open(&SDFile,filename_id,FA_OPEN_EXISTING|FA_READ);
	if(FR_OK==retSD)
			LCD_ShowLine(3,"Open Id Successfully");
	else LCD_ShowLine(3,"Open Id Error");
	//第一行数据舍弃
	f_gets(rBuff,1024,&SDFile);
	//读取一行数据
	while(f_gets(rBuff,1024,&SDFile)!=0)
	{
		point=0;
		while(rBuff[point]!=',')point++;
		point++;
		
		//读取校园卡卡号
		for(i=0;i<10;i++)
		{
			id_number[line][i]=rBuff[point];
			point++;
		}
		line++;
	}
	//关闭文件
	f_close(&SDFile);
}

void Load_Data(struct WZ * wz, char * filename)
{
	char rBuff[1024];//读取数据暂存
	u16 point=0;
	u16 line=0;
	u8 i;
	//打开文件
	retSD=f_open(&SDFile,filename,FA_OPEN_EXISTING|FA_READ);
	if(FR_OK==retSD)
			LCD_ShowLine(4,"Open Data Successfully");
	else LCD_ShowLine(4,"Open Data Error");
	//第一行数据舍弃
	f_gets(rBuff,1024,&SDFile);
	//读取一行数据
	while(f_gets(rBuff,1024,&SDFile)!=0)
	{
		point=0;
		//读取标签编号
		for(i=0;i<8;i++)
		{
			wz[line].id[i]=rBuff[point];
			point++;
		}
		point++;
		while(rBuff[point]!=',')point++;
		point++;
		if(rBuff[point])//已经借出了
		{
			wz[line].jie=1;
			point+=2;
			for(i=0;i<10;i++)
			{
				wz[line].jie_id[i]=rBuff[point];
				point++;
			}
		}
		else wz[line].jie=0;
		line++;
	}
	//关闭文件
	f_close(&SDFile);
}

void main_page()
{
	u8 key;
	LCD_Clear(BLACK);
	LCD_ShowLine(1,"Press 'A' to borrow");
	LCD_ShowLine(2,"Press 'B' to return");
	LCD_ShowLine(3,"Press 'C' to query items");
	while(1)
	{
		key=JzKey();
		switch (key)
    {
    	case 4://borrow
				if(read_id())
				{
					LCD_Clear(BLACK);
					LCD_ShowLine(1,"Welcome");
					HAL_Delay(3000);
					return;
				}
				else
				{
					LCD_Clear(BLACK);
					LCD_ShowLine(1,"No permission");
					HAL_Delay(3000);
					return;
				}
    	case 8:break;
			case 12:break;
    }
	}
}

u8 read_id()
{
	u8 i;
	//将卡号读入全局变量中...
	for(i=0;i<200;i++)
		if(!strcmp(id_number[i], card_id))return 1;//存在相同的学号
	return 0;//没有该学号
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
