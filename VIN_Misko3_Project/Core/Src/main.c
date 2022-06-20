/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "crc.h"
#include "dac.h"
#include "dma.h"
#include "fdcan.h"
#include "i2c.h"
#include "quadspi.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fmc.h"
#include "usb.h"
#include "stdio.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "LED.h"
#include "kbd.h"
#include "sci.h"
#include "lcd.h"
#include "ugui.h"
#include "XPT2046_touch.h"
#include "ColorSpaces.h"
#include "joystick.h"
#include "mazeGenerator.h"
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


void UserPixelSetFunction(UG_S16 x, UG_S16 y, UG_COLOR c)
{
	ILI9341_SetDisplayWindow(x, y, 1, 1);
	ILI9341_SendData((LCD_IO_Data_t *)&c, 1);
}


UG_GUI gui;
int swidth;
int sheight;
uint16_t touch_x = 0, touch_y = 0;
coord_t joystick_out;

int joystickSensitivity = 17;

// Main menu
int pages = 3;
uint8_t state = 0;
uint8_t menuSelect = 0;

// Maze

int menu[] = {
		1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,0,0,0,1,0,0,0,0,0,0,0,1,
		1,0,1,0,1,1,1,0,1,1,1,0,1,
		1,0,1,0,0,0,0,0,1,0,0,0,1,
		1,0,1,0,1,1,1,1,1,1,1,0,1,
		1,0,1,0,0,0,0,0,0,0,0,0,1,
		1,0,1,1,1,0,1,1,1,0,1,1,1,
		1,0,0,0,1,0,1,0,0,0,1,0,1,
		1,0,1,0,1,0,1,0,1,0,1,0,1,
		1,0,1,0,0,0,1,0,1,0,1,0,1,
		1,0,1,1,1,1,1,0,1,1,1,0,1,
		1,0,0,0,0,0,1,0,0,0,0,0,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1
};

int* maze;
int maze_playerLocation = 0;
int maze_playerSize = 5;
int maze_cellSize = 21;
int maze_sizeX = 29;
int maze_sizeY = 19;
int maze_offsetX = 12;
int maze_offsetY = 40;

char timer[8];
int time = 0;

void MainMenuRefresh(){
	char index[2];
	sprintf(index, "%d", menuSelect+1);
	UG_FillFrame( 71 , 41 , 249 , 219, C_BLACK);
	UG_FontSelect(&FONT_16X26);
	UG_PutString(74,44,index);
	switch(menuSelect){
		case 0:
			UG_DrawRoundFrame( 70 , 40 , 250 , 220 , 10, C_WHITE);
			drawMaze(menu,13,13,97,70,21,0);
			UG_PutString(126,109,"EASY\nMAZE");
			maze_cellSize = 31;
			maze_sizeX = 21;
			maze_sizeY = 13;
			maze_playerSize = 7;
			maze_offsetX = 5;
			maze_offsetY = 43;
			break;
		case 1:
			UG_DrawRoundFrame( 70 , 40 , 250 , 220 , 10, C_WHITE);
			drawMaze(menu,13,13,97,70,21,0);
			UG_PutString(109,109,"NORMAL\n MAZE");
			maze_cellSize = 21;
			maze_sizeX = 29;
			maze_sizeY = 19;
			maze_playerSize = 5;
			maze_offsetX = 12;
			maze_offsetY = 40;
			break;
		case 2:
			UG_DrawRoundFrame( 70 , 40 , 250 , 220 , 10, C_WHITE);
			drawMaze(menu,13,13,97,70,21,0);
			UG_PutString(126,109,"HARD\nMAZE");
			maze_cellSize = 12;
			maze_sizeX = 51;
			maze_sizeY = 33;
			maze_playerSize = 2;
			maze_offsetX = 10;
			maze_offsetY = 40;
			break;
		default:
			UG_DrawRoundFrame( 70 , 40 , 250 , 220 , 10, C_BLUE);
			UG_PutString(126,115,"SOON");
			break;
	}
}


void MainMenu(){
	UG_FillScreen(C_BLACK);
	UG_SetForecolor(C_WHITE);
	UG_SetBackcolor(C_BLACK);
	UG_FontSelect(&FONT_16X26);
	UG_PutString(126,0,"MENU");
	UG_SetBackcolor(C_BLUE);
	UG_FillFrame( 0 , 0 , 60 , sheight , C_BLUE);
	UG_PutString(20,110,"<");
	UG_FillFrame( 260 , 0 , 320 , sheight , C_BLUE);
	UG_PutString(284,110,">");
	UG_SetBackcolor(C_BLACK);
	MainMenuRefresh();
}

void setPlayerLocation(){
	maze_playerLocation = maze_sizeX*(maze_sizeY-2)+1;
}

void mazeControls(int posX, int posY){
	time++;
	int locX = maze_playerLocation%maze_sizeX/2*maze_cellSize + maze_offsetX + (int)maze_cellSize/2+1;
	int locY = (int)(maze_playerLocation/(maze_sizeX*2))*maze_cellSize + maze_offsetY + (int)maze_cellSize/2+1;
	UG_FillCircle(locX, locY,  maze_playerSize , C_BLACK);
	maze_playerLocation+=posX;
	maze_playerLocation+=posY*maze_sizeX;
	if(maze_playerLocation<0 || maze_playerLocation > maze_sizeX*maze_sizeY-1 || maze[maze_playerLocation]==1 ){
		maze_playerLocation-=posX;
		maze_playerLocation-=posY*maze_sizeX;
		UG_FillCircle(locX, locY,  maze_playerSize , C_YELLOW);
		return;
	}
	maze_playerLocation+=posX;
	maze_playerLocation+=posY*maze_sizeX;
	locX = maze_playerLocation%maze_sizeX/2*maze_cellSize + maze_offsetX + (int)maze_cellSize/2+1;
	locY = (int)(maze_playerLocation/(maze_sizeX*2))*maze_cellSize + maze_offsetY + (int)maze_cellSize/2+1;
	UG_FillCircle(locX,  locY ,  maze_playerSize , C_YELLOW);
	if(maze_playerLocation == maze_sizeX*2-2 ) {
		maze = getMaze(maze_sizeX,maze_sizeY);
		UG_FillFrame( 98 , 0 , swidth , 30 , C_BLUE);
		UG_FillFrame( 0 , 30 , swidth , sheight , C_BLACK);
		setPlayerLocation();
		drawMaze(maze,maze_sizeX,maze_sizeY,maze_offsetX,maze_offsetY,maze_cellSize,1);
		mazeControls(0,0);
		time = 0;
	}
}

void staticContent(){
	UG_FillScreen(C_BLACK);
	switch(state){
	case 1:
	case 2:
	case 3:
			UG_FillFrame( 0 , 0 , swidth , 30 , C_BLUE);
			UG_SetForecolor(C_WHITE);
			UG_SetBackcolor(C_BLUE);
			UG_PutString(0,1,"Moves: ");
			UG_FontSelect(&FONT_16X26);
			maze = getMaze(maze_sizeX,maze_sizeY);
			setPlayerLocation();
			drawMaze(maze,maze_sizeX,maze_sizeY,maze_offsetX,maze_offsetY,maze_cellSize,1);
			mazeControls(0,0);
			time = 0;
		break;
	default:
			state = 0;
			MainMenu();
			break;
	}
}

void touchInput(){
	if(state < 1 || state > 5){
		if(menuSelect >0 && touch_x < (unsigned) 60 ){
				menuSelect--;
				MainMenuRefresh();
		}else if(menuSelect < pages && touch_x > (unsigned)260){
				menuSelect++;
				MainMenuRefresh();
		}else{
			state = menuSelect+1;
			staticContent();
		}
		 HAL_Delay(150);
	}
}

int x = 0;
int y = 0;

void inputControls()
{
	if(!KBD_get_button_state(BTN_ESC)) state = 0;

	switch(state){
		case 1:
		case 2:
		case 3:
			x = 0; y = 0;
			sprintf(timer, "%d", time);
			UG_PutString(98,1,timer);
			if(!KBD_get_button_state(BTN_LEFT) || joystick_out.x < -joystickSensitivity) x = -1;
			else if(!KBD_get_button_state(BTN_RIGHT) || joystick_out.x > joystickSensitivity) x = 1;
			else if(!KBD_get_button_state(BTN_UP) || joystick_out.y < -joystickSensitivity) y = -1;
			else if(!KBD_get_button_state(BTN_DOWN) || joystick_out.y > joystickSensitivity) y = 1;
			if(x!= 0 || y != 0){
				mazeControls(x,y);
				HAL_Delay(150);
			}
			break;
		default:
			if(menuSelect>0 && (!KBD_get_button_state(BTN_LEFT) || joystick_out.x < -joystickSensitivity)){
				menuSelect--;
				MainMenuRefresh();
				HAL_Delay(150);
			}
			else if(menuSelect<pages && (!KBD_get_button_state(BTN_RIGHT) || joystick_out.x > joystickSensitivity)){
				menuSelect++;
				MainMenuRefresh();
				HAL_Delay(150);
			}
			else if(!KBD_get_button_state(BTN_OK) || !KBD_get_button_state(BTN_JOY)){
				state = menuSelect+1;
				staticContent();
			}
			break;
	}
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  *
  */

int main(void)
{
  /* USER CODE BEGIN 1 */
	coord_t joystick_raw;
	joystick_t joystick;

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
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_FMC_Init();
  MX_I2C2_Init();
  MX_UART4_Init();
  MX_UART5_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_QUADSPI1_Init();
  MX_SPI1_Init();
  MX_TIM5_Init();
  MX_TIM8_Init();
  MX_TIM20_Init();
  MX_ADC3_Init();
  MX_DAC1_Init();
  MX_DAC2_Init();
  MX_FDCAN2_Init();
  MX_I2C1_Init();
  MX_TIM15_Init();
  MX_USART3_UART_Init();
  MX_ADC4_Init();
  MX_DMA_Init();
  MX_CRC_Init();
  MX_TIM6_Init();
  MX_USB_PCD_Init();
  /* USER CODE BEGIN 2 */

  LED_init();
  KBD_init();
  SCI_init();
  joystick_init(&joystick, -6, 6);


  LCD_Init();
  swidth = ILI9341_GetParam(LCD_WIDTH);
  sheight = ILI9341_GetParam(LCD_HEIGHT);
//320x240

  UG_Init(&gui, UserPixelSetFunction, swidth, sheight);
  UG_FontSelect(&FONT_8X12);
  UG_SetForecolor(C_WHITE);
  UG_SetBackcolor(C_BLACK);

  MainMenu();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
	/* USER CODE END WHILE */

	/* USER CODE BEGIN 3 */

	HAL_ADC_Start(&hadc4);
	HAL_ADC_PollForConversion(&hadc4,10);// Waiting for ADC conversion
	joystick_raw.x=HAL_ADC_GetValue(&hadc4);

	HAL_ADC_Start(&hadc4);
	HAL_ADC_PollForConversion(&hadc4,10);// Waiting for ADC conversion
	joystick_raw.y=HAL_ADC_GetValue(&hadc4);
	HAL_ADC_Stop(&hadc4);

	joystick_get(&joystick_raw, &joystick_out, &joystick);
	//printf("x:%d y:%d\r\n",joystick_out.x,joystick_out.y); // 115200

	if(XPT2046_TouchPressed()) {
		uint16_t x = 0, y = 0;
		if(XPT2046_TouchGetCoordinates(&x, &y, 0)) {
			touch_x = x;
			touch_y = y;
			touchInput();
		//UG_FillCircle(250, 50, 49, C_BLACK);
		}
	}

	LED_set(LED0, !KBD_get_button_state(BTN_UP));
	LED_set(LED1, !KBD_get_button_state(BTN_RIGHT));
	LED_set(LED2, !KBD_get_button_state(BTN_DOWN));
	LED_set(LED3, !KBD_get_button_state(BTN_LEFT));

	LED_set(LED4, !KBD_get_button_state(BTN_OK));
	LED_set(LED5, !KBD_get_button_state(BTN_ESC));
	LED_set(LED6, (joystick_out.x > joystickSensitivity || joystick_out.x < -joystickSensitivity || joystick_out.y > joystickSensitivity || joystick_out.y < -joystickSensitivity));
	LED_set(LED7, !KBD_get_button_state(BTN_JOY));

	inputControls();
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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV2;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV6;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
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
