/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "i2c.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
#include "font.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
#define OLED
//HCSR04 //OLED


#ifdef HCSR04
	int upEdge = 0;//获取上升沿时刻计数值
	int downEdge = 0;//获取下降沿时刻计数值
	float distance = 0;
#endif

#ifdef PWM
	uint16_t count = 150;
#endif


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#ifdef HCSR04
// 输入捕获中断回调函数
	void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
	{
	  if(htim == &htim1 && htim -> Channel == HAL_TIM_ACTIVE_CHANNEL_4)
	  {
		  //使用捕获寄存器数值的函数HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
		  upEdge = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
		  downEdge = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
		  distance = ((downEdge - upEdge) * 0.034) / 2 ;
	  }
	}
#endif
#ifdef KEY
	void EXTI15_10_IRQHandler(void)
	{
	 HAL_Delay(10);
	 if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_12)==GPIO_PIN_RESET)
	 {

	 }

	}
#endif
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
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
#ifdef OLED
  HAL_Delay(20);
  OLED_Init();
#endif

#ifdef HCSR04
  HAL_Delay(20);
  OLED_Init();

  HAL_TIM_Base_Start(&htim1);//启动计时器计数

  //启用定时器对应通道的输入捕获功能
  HAL_TIM_IC_Start(&htim1,TIM_CHANNEL_3);//开启通道3的输入捕获
  //启动输入捕获
  HAL_TIM_IC_Start_IT(&htim1,TIM_CHANNEL_4);//开启通道4的输入捕获,启用定时器对应通道的输入捕获功能
   char message [50] = "";
#endif
#ifdef PWM
	HAL_TIM_PWM_Init(&htim2);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
#endif


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
#ifdef PWM
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,count);
#endif
  while (1)
  {
	#ifdef OLED
	  OLED_NewFrame();
	  OLED_PrintString(0, 0,"abc",&font16x16, OLED_COLOR_NORMAL);
	  OLED_ShowFrame();
	#endif

	#ifdef HCSR04
	  	  //1  首先触发超声模块测距，先将Trig拉高
	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
	  	  //经过一段时间在拉低
	  	  HAL_Delay(1);
	  	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
	  	  //触发计数器时清零，我们就不用考虑doemEdge > upEdge 的情况了
	  	  __HAL_TIM_SET_COUNTER(&htim1,0);
	  	  HAL_Delay(20);

	  	  //3 数据展示
	  	  //下来把数据显示在屏幕上
	  	  //先套上新建针和显示帧的函数

	  	  OLED_NewFrame();
	  	  sprintf(message,"距离 = %.2fcm",distance);
	  	  OLED_PrintString(0, 0, message, &font16x16, OLED_COLOR_NORMAL);
	  	  OLED_ShowFrame();

	  	  HAL_Delay(500);
	#endif

	#ifdef PWM
	  	/*for(int i=0;i<4;i++)
	  		  {
	  			  count += 10;
	  			  HAL_Delay(1000);
	  			  __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,count);
	  		  }
	  		  for(int i=0;i<4;i++)
	  		  {
	  			  count -= 10;
	  			  HAL_Delay(1000);
	  			  __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,count);
	  		  }*/
//	  	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,count);
	#endif

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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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
#ifdef USE_FULL_ASSERT
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
