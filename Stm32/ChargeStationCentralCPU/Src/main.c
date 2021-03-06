
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "lwip.h"
#include "usb_device.h"

/* USER CODE BEGIN Includes */

#include "display.h"
#include "RFID.h"
#include "rc522.h"
#include "channel.h"
#include "tasks.h"
//#include "usbd_cdc_if.h"
#include "settings.h"
#include "rng.h"
#include "serial_control.h"
#include "chargePointTime.h"
#include "net.h"
#include "netConn.h"
#include "ocpp.h"
#include "ocppConfiguration.h"
#include "ocppConfigurationDef.h"
#include "connector.h"
#include "chargePoint.h"
#include "indication.h"
#include "localAuthList.h"
#include "chargePointState.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c2;
DMA_HandleTypeDef hdma_i2c2_rx;
DMA_HandleTypeDef hdma_i2c2_tx;

RNG_HandleTypeDef hrng;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi4;

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart2_tx;

osThreadId defaultTaskHandle;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

QueueHandle_t mainQueue;
uint8_t str[16];
uint32_t lastUserButtonPressTick = 0;
ChargePointSetting *settings;
bool isNeedSetTime = true;
idToken lastCheckedTagId;
uint32_t lastCheckedTagTick = 0;

OcppConfigurationVaried ocppConfVaried;
OcppConfigurationFixed ocppConfFixed;
OcppConfigurationRestrict ocppConfRestrict;
ChargePointConnector connector[CONFIGURATION_NUMBER_OF_CONNECTORS];

bool remoteStartTransactionProcessing = false;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_I2C2_Init(void);
static void MX_SPI4_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_RNG_Init(void);
static void MX_RTC_Init(void);
void StartDefaultTask(void const * argument);

/* USER CODE BEGIN PFP */
#define PACK_TO_PARAM_BYTE0(param, value) param |= (value & 0xFF)
#define PACK_TO_PARAM_BYTE1(param, value) param |= ((value & 0xFF) << 8)
#define PACK_TO_PARAM_BYTE2(param, value) param |= ((value & 0xFF) << 16)
#define PACK_TO_PARAM_BYTE3(param, value) param |= ((value & 0xFF) << 24)

/* USER CODE END PFP */

#define CAP_CARD_DECLINED "CARD DECLINED"
#define CAP_CARD_ACCEPTED "CARD ACCEPTED"
#define MES_NO_CONNECTION "NO CONNECTION"

/* USER CODE BEGIN 0 */


void initDisplay(){
	Display_init(&hi2c2);
	
	Display_PrintStrCenter(0, "Starting\0");
	Display_PrintStrCenter(1, "Hardware checking\0");
}

void initConnectors(){
	int i;
	for(i = 0; i < CONFIGURATION_NUMBER_OF_CONNECTORS; i++){
		connector[i].id = i + 1;
		connector[i].address = i + 1;
		connector[i].status = CHARGE_POINT_STATUS_UNKNOWN;
		connector[i].online = false;
		connector[i].meterValue = 0;
		connector[i].isMeterValueSet = false;
		connector[i].isMeterValueRequest = true;
		connector[i].pilotSygnalLevel = PILOT_SYGNAL_LEVEL_F;
		connector[i].pilotSygnalPwm = PILOT_SYGNAL_PWM_UNKNOWN;
		connector[i].noAnswerCnt = 0;
		
		connector[i].chargeTransaction.isActive = false;
		connector[i].chargeTransaction.isClosed = true;
		connector[i].chargeTransaction.stopReason = -1;
	}
}

void checkHardware(){
	
	//Init RFID
	RFID_init(&hspi4);
	if(!(RFID_check_connection())){
		Display_PrintStrLeft(1, "ERROR: NO RFID\0");
		ChargePoint_setRFIDEnabled(false);
	}
	
	//Init connector
	
	Channel_init(&huart2);
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

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
  MX_USART3_UART_Init();
  MX_I2C2_Init();
  MX_SPI4_Init();
  MX_USART2_UART_Init();
  MX_RNG_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
	
	printf("Start ChargeStationCentralCPU\n");

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
	
  mainQueue = xQueueCreate(32, sizeof(GeneralMessage));
	
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
 

  /* Start scheduler */
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

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

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/* I2C2 init function */
static void MX_I2C2_Init(void)
{

  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* RNG init function */
static void MX_RNG_Init(void)
{

  hrng.Instance = RNG;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* RTC init function */
static void MX_RTC_Init(void)
{

  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;

    /**Initialize RTC Only 
    */
  hrtc.Instance = RTC;
if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0) != 0x32F2){
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initialize RTC and set the Time and Date 
    */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR0,0x32F2);
  }

}

/* SPI4 init function */
static void MX_SPI4_Init(void)
{

  /* SPI4 parameter configuration*/
  hspi4.Instance = SPI4;
  hspi4.Init.Mode = SPI_MODE_MASTER;
  hspi4.Init.Direction = SPI_DIRECTION_2LINES;
  hspi4.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi4.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi4.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi4.Init.NSS = SPI_NSS_SOFT;
  hspi4.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi4.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi4.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi4.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi4.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART2 init function */
static void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 38400;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART3 init function */
static void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
  /* DMA1_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
  /* DMA1_Stream7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream7_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream7_IRQn);

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(RFID_CS_GPIO_Port, RFID_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD3_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : RFID_CS_Pin */
  GPIO_InitStruct.Pin = RFID_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(RFID_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USER_Btn_Pin */
  GPIO_InitStruct.Pin = USER_Btn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_Btn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD3_Pin LD2_Pin */
  GPIO_InitStruct.Pin = LD3_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = USB_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OverCurrent_Pin */
  GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

void toggleBlueLed(void){
	HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
}

bool checkChargeTransactionByTag(idToken tagId){
	int i;
	char s[32];
	ChargePointConnector *conn;
	//Check if someony with this tag is charging
	for(i = 0; i < CONFIGURATION_NUMBER_OF_CONNECTORS; i++){
		conn = &connector[i];
		if(strncmp(conn->userTagId, tagId, ID_TOKEN_SIZE) == 0){
			if(conn->status != CHARGE_POINT_STATUS_FINISHING){
				sprintf(s, "Channel %d", i+1);
				Indication_ShowMessage("Finish charging", s, 2000);
				conn->chargeTransaction.stopReason = OCPP_REASON_LOCAL;
				Channel_haltCharging(i);
			}
			return true;
		}
	}
	
	return false;
}

bool checkTagInLocalAuthList(idToken tagId){
	AuthorizationData *data;
	data = localAuthList_getDataByTag(tagId);
	if(data == NULL)
		return false;
	return (data->idTagInfo.status == AUTHORIZATION_STATUS_ACCEPTED);
}

void startChargeTransaction(idToken tagId, int connectorId){
	ChargePointConnector *conn;
	NetInputMessage netMessage;
	int i, iStart, iEnd;
	if(connectorId == -1){
		iStart = 0;
		iEnd = CONFIGURATION_NUMBER_OF_CONNECTORS;
	}
	else{
		iStart = connectorId;
		iEnd = iStart + 1;
	}
	
	for(i = iStart; i < iEnd; i++){
		conn = &connector[i];
		if(conn->status == CHARGE_POINT_STATUS_PREPARING){
			
			strcpy(conn->userTagId, tagId);
			conn->chargeTransaction.startMeterValue = conn->meterValue;
	    conn->chargeTransaction.isActive = true;
	    conn->chargeTransaction.isClosed = false;
	
			if(cpState_isServerOnline()){
				netMessage.messageId = NET_INPUT_MESSAGE_START_TRANSACTION;
				netMessage.param1 = (int)conn;
				NET_sendInputMessage(&netMessage);
			}
	
	    Channel_startCharging(i);
			
			return;
		}
	}
}

void stopChargeTransaction(int channel){
	ChargePointConnector *conn;
	NetInputMessage netMessage;
	
	conn = &connector[channel];
	conn->chargeTransaction.isClosed = true;
	conn->chargeTransaction.stopMeterValue = conn->meterValue;
	
	netMessage.messageId = NET_INPUT_MESSAGE_STOP_TRANSACTION;
	netMessage.param1 = (int)conn;
	NET_sendInputMessage(&netMessage);
}

void sendUnlockConnectorAnswer(int status, int uniqIdIndex){
	NetInputMessage netMessage;
	netMessage.messageId = NET_INPUT_MESSAGE_UNLOCK_CONNECTOR_ANSWER;
	netMessage.param1 = status;
	netMessage.uniqIdIndex = uniqIdIndex;
	NET_sendInputMessage(&netMessage);
}



void acceptAuth(idToken tagId, bool success){
	char s[32];
	sprintf(s, "Authorize %s", success ? "SUCCESS" : "FAILED");
	printf("%s\n", s);
	Indication_ShowMessage(s, NULL, 5000);	
	
	if(success){
		startChargeTransaction(tagId, -1);
	}
}

void makeReboot(void){
	HAL_NVIC_SystemReset();
}

void processChannelFullCycleNotify(int reason){
	if(reason == FULL_CYCLE_NOTIFY_REASON_REBOOT){
		makeReboot();
	}
}

void processEnabledTag(idToken cardId){
	NetInputMessage netMessage;
	char s[32];
	bool needNetworkCheck = false;
	
	printf("CardId %s\n", cardId);
		
	if((strncmp(cardId, lastCheckedTagId, ID_TOKEN_SIZE) == 0) && ((HAL_GetTick() - lastCheckedTagTick) < 5000))
		return;
	strcpy(lastCheckedTagId, cardId);
	lastCheckedTagTick = HAL_GetTick();
	
	sprintf(s, "CardId %s", cardId);
	Indication_ShowMessage("Card discovered", s, 5000);
				
	if(checkChargeTransactionByTag(cardId))
		return;
	
	if(cpState_isServerOnline()){
		if(ocppConfVaried.localPreAuthorize){
			if(!checkTagInLocalAuthList(cardId))
				needNetworkCheck = true;
		}
		else{
			needNetworkCheck = true;
		}
	}
	else{
		if((!ocppConfVaried.localAuthorizeOffline) || (!checkTagInLocalAuthList(cardId)) ){
				Indication_ShowMessage(CAP_CARD_DECLINED, MES_NO_CONNECTION, 5000);
				return;
		}
	}
	
	if(needNetworkCheck){
		netMessage.messageId = NET_INPUT_MESSAGE_AUTHORIZE;
		strcpy(netMessage.tagId, cardId);
		NET_sendInputMessage(&netMessage);
	}
	else{
		// Just start charging
		startChargeTransaction(cardId, -1);
		Indication_ShowMessage(CAP_CARD_ACCEPTED, NULL, 5000);
	}
}

void sendRemoteStartTransactionAnswer(int status, int uniqIdIndex){
	NetInputMessage netMessage;
	netMessage.messageId = NET_INPUT_MESSAGE_REMOTE_START_TRANSACTION_ANSWER;
	netMessage.param1 = status;
	netMessage.uniqIdIndex = uniqIdIndex;
	NET_sendInputMessage(&netMessage);
}

void sendRemoteStopTransactionAnswer(int status, int uniqIdIndex){
	NetInputMessage netMessage;
	netMessage.messageId = NET_INPUT_MESSAGE_REMOTE_STOP_TRANSACTION_ANSWER;
	netMessage.param1 = status;
	netMessage.uniqIdIndex = uniqIdIndex;
	NET_sendInputMessage(&netMessage);
}

void sendChangeAvailabilityAnswer(int status, int uniqIdIndex){
	NetInputMessage netMessage;
	netMessage.messageId = NET_INPUT_MESSAGE_CHANGE_AVAILABILITY;
	netMessage.param1 = status;
	netMessage.uniqIdIndex = uniqIdIndex;
	NET_sendInputMessage(&netMessage);
}

void sendClearCacheAnswer(int status, int uniqIdIndex){
	NetInputMessage netMessage;
	netMessage.messageId = NET_INPUT_MESSAGE_CLEAR_CACHE;
	netMessage.param1 = status;
	netMessage.uniqIdIndex = uniqIdIndex;
	NET_sendInputMessage(&netMessage);
}


void remoteStartTransaction(GeneralMessage *message){
	bool authRemoteTxRequests;
	int uniqIdIndex;
	int connectorId;
	idToken idTag;
	
	connectorId = message->param1;
	uniqIdIndex = message->param2;
	
	if(remoteStartTransactionProcessing || (connectorId < 0) || (connectorId > CONFIGURATION_NUMBER_OF_CONNECTORS)) {
		//Previous operation is not processed yet or invalid connector Id
		//reject new operation
		sendRemoteStartTransactionAnswer(REMOTE_STARTSTOP_STATUS_REJECTED, uniqIdIndex);
		return;
	}
		
	
	authRemoteTxRequests = ocppConfRestrict.authorizeRemoteTxRequestsReadOnly ? ocppConfFixed.authorizeRemoteTxRequests:  ocppConfVaried.authorizeRemoteTxRequests;
	if(authRemoteTxRequests){
		
	}
	
	strcpy(idTag, message->tokenId);
	sendRemoteStartTransactionAnswer(REMOTE_STARTSTOP_STATUS_ACCEPTED, uniqIdIndex);
	startChargeTransaction(idTag, connectorId - 1); 
}

void remoteStopTransaction(GeneralMessage *message){
	int uniqIdIndex;
	int transactionId;
	bool taIsFound;
	ChargePointConnector *conn;
	int i;
	
	transactionId = message->param1;
	uniqIdIndex = message->param2;
	
	taIsFound = false;
	for(i = 0; i < CONFIGURATION_NUMBER_OF_CONNECTORS; i++){
		conn = &connector[i];
		if((conn->chargeTransaction.id == transactionId) && conn->chargeTransaction.isActive){
			taIsFound = true;
			break;
		}
	}
	
	if(taIsFound){
		sendRemoteStopTransactionAnswer(REMOTE_STARTSTOP_STATUS_ACCEPTED, uniqIdIndex);
		Indication_ShowMessage("Remote finish", "of charging", 2000);
		conn->chargeTransaction.stopReason = OCPP_REASON_REMOTE;
		Channel_haltCharging(i);
		//stopChargeTransaction(i);
	}
	else{
		sendRemoteStopTransactionAnswer(REMOTE_STARTSTOP_STATUS_REJECTED, uniqIdIndex);
	}
}



void remoteReset(GeneralMessage *message){
	int resetType;
	int i;
	ChargePointConnector *conn;

	resetType = message->param1;
	
	/*if(resetType == RESET_TYPE_HARD){
		resetStatus = OCPP_RESET_STATUS_ACCEPTED;
	}*/
	
	for(i = 0; i < CONFIGURATION_NUMBER_OF_CONNECTORS; i++){
		conn = &connector[i];
		conn->isNeedReset = true;
	}
	
	Channel_requestFullCycleNotify(FULL_CYCLE_NOTIFY_REASON_REBOOT);
	
	//makeReboot();
}

void changeAvalability(GeneralMessage *message){
	int availType;
	int connId;
	int uniqIdIndex;
	
	availType = message->param1 & 0xFF;
	connId = (message->param1 >> 8) & 0xFF; 
	uniqIdIndex = message->param2;
	
	sendChangeAvailabilityAnswer(OCPP_AVAILABILITY_STATUS_REJECTED, uniqIdIndex);
}

void clearAuthCache(GeneralMessage *message){
	int uniqIdIndex;
	int status;
	
	uniqIdIndex = message->param2;
	
	status = OCPP_CLEAR_CACHE_STATUS_REJECTED;
	sendClearCacheAnswer(status, uniqIdIndex);
}

void processMessageFromNET(GeneralMessage *message){
	NetInputMessage netMessage;
	printf("Main task. Message from NET is got\n");
	char s[32];
	switch(message->messageId){
		case MESSAGE_NET_SERVER_ACCEPT:
			sprintf(s, "Station %s", message->param1 ? "ACCEPTED" : "REJECTED");
		  Indication_ShowMessage(s, NULL, 5000);
		
		  if(NET_is_station_accepted()){
				//When station is accepted. It is need to send charge point status to server
				netMessage.messageId = NET_INPUT_MESSAGE_SEND_CHARGE_POINT_STATUS;
				NET_sendInputMessage(&netMessage);
			}
			cpState_setServerOnline(NET_is_station_accepted());
			break;
		case MESSAGE_NET_AUTHORIZE:
			acceptAuth(message->tokenId, message->param1);
			break;
		case MESSAGE_NET_UNLOCK_CONNECTOR:
			Channel_unlockConnector(message->param1 - 1, message->param2);
			break;
		case MESSAGE_NET_REMOTE_START_TRANSACTION:
			remoteStartTransaction(message);
			break;
		case MESSAGE_NET_REMOTE_STOP_TRANSACTION:
			remoteStopTransaction(message);
			break;
		case MESSAGE_NET_RESET:
			remoteReset(message);
			break;
		case MESSAGE_NET_CHANGE_AVAILABILITY:
			changeAvalability(message);
			break;
		case MESSAGE_NET_CLEAR_CACHE:
			clearAuthCache(message);
			break;
	}
}

void onChannelStatusChanged(ChargePointConnector *conn){
	NetInputMessage netMessage;
	
	//If connection with server is present it is need to send new status 
	if(NET_is_station_accepted()){
		netMessage.messageId = NET_INPUT_MESSAGE_SEND_CONNECTOR_STATUS;
		netMessage.param1 = 0;
		PACK_TO_PARAM_BYTE0(netMessage.param1, conn->id);
		PACK_TO_PARAM_BYTE1(netMessage.param1, conn->status);
		PACK_TO_PARAM_BYTE2(netMessage.param1, CHARGE_POINT_ERROR_CODE_NO_ERROR);
		NET_sendInputMessage(&netMessage);
	}
	
	switch(conn->status){
		case CHARGE_POINT_STATUS_AVAILABLE:
			strcpy(conn->userTagId, "");
      strcpy(lastCheckedTagId, "");		
			break;
		case CHARGE_POINT_STATUS_CHARGING:
			strcpy(lastCheckedTagId, "");	
			break;
		case CHARGE_POINT_STATUS_SUSPENDED_EV:
			conn->isMeterValueRequest = true;
			break;
	}
	Indication_PrintChannel(conn->id - 1, conn);
}

void finishUnlockConnector(GeneralMessage *message){
	int uniqIdIndex;
	int status;
	ChargePointConnector *conn;
	
	conn = (ChargePointConnector*)message->param1;
	uniqIdIndex = conn->uniqMesIndUnlockConnector;
	
	status = conn->isLocked ? UNLOCK_STATUS_UNLOCK_FAILED : UNLOCK_STATUS_UNLOCKED;
	
	sendUnlockConnectorAnswer(status, uniqIdIndex);
	
	if((!conn->isLocked) && conn->chargeTransaction.isActive){
		//It is need to close transaction
		conn->status = CHARGE_POINT_STATUS_FINISHING;
		conn->isMeterValueRequest = true; //Need request meterValue for close transaction
		conn->chargeTransaction.stopReason = OCPP_REASON_UNLOCK_COMMAND;
		conn->chargeTransaction.isActive = false;
		onChannelStatusChanged(conn);
	}
}

void processMessageFromChannels(GeneralMessage *message){
	int connIndex;
	ChargePointConnector *conn;
	
	switch(message->messageId){
		case MESSAGE_CHANNEL_STATUS_CHANGED:
			connIndex = message->param1;
		  conn = &connector[connIndex];
			onChannelStatusChanged(conn);
			break;
		case MESSAGE_CHANNEL_CHARGING_HALTED:
			connIndex = message->param1;
		  conn = &connector[connIndex];
		  conn->status = CHARGE_POINT_STATUS_FINISHING;
		  conn->isMeterValueRequest = true; //Need request meterValue for close transaction
		  conn->chargeTransaction.isActive = false;
		  onChannelStatusChanged(conn);
			break;
		case MESSAGE_CHANNEL_GET_METER_VALUE:
			connIndex = message->param1;
		  conn = &connector[connIndex];
		  if(conn->chargeTransaction.isActive){
				//Need update screen
				Indication_PrintChannel(conn->id - 1, conn);
			}
			else{
				if(!conn->chargeTransaction.isClosed){
					//CloseTransaction
					stopChargeTransaction(connIndex);
				}
			}
			break;
		case MESSAGE_CHANNEL_UNLOCK_CONNECTOR:
			finishUnlockConnector(message);
			break;
		case MESSAGE_CHANNEL_FULL_CYCLE_NOTIFY:
			processChannelFullCycleNotify(message->param1);
			break;	
	}
}

void processMessageFromRFID(GeneralMessage *message){

	NetInputMessage netMessage;
	idToken tagId;
	
	printf("Main task. Message from RFID is got\n");
	switch(message->messageId){
		case MESSAGE_RFID_FOUND_CARD:
			sprintf(tagId, "%.8X", message->param1);
			processEnabledTag(tagId);
			break;
		case MESSAGE_RFID_CONNECTION:
			if(message->param1){
				//Connected
				ChargePoint_setRFIDEnabled(true);
			}
			else{
				//Disconnected
				ChargePoint_setRFIDEnabled(false);
			}
			netMessage.messageId = NET_INPUT_MESSAGE_SEND_CHARGE_POINT_STATUS;
			NET_sendInputMessage(&netMessage);
			break;
	}
}

void processMessageFromSerialControl(GeneralMessage *message){
	printf("Main task. Message from SerialControl is got\n");
	ChargePointSetting* st;	
	switch(message->messageId){
		case MESSAGE_SER_CONTROL_SET_LOCAL_IP:
			NET_changeLocalIp();
			break;
		case MESSAGE_SER_CONTROL_SET_SERVER_HOST:
			st = Settings_get();
			NET_CONN_setRemoteHost(st->serverHost);
			break;
		case MESSAGE_SER_CONTROL_SET_SERVER_PORT:
			st = Settings_get();
			NET_CONN_setRemotePort(st->serverPort);
			break;
	}
}

void testSaveSetting(){
	printf("testSaveSetting\n");
	sprintf(settings->ChargePointId, "SaveCP_%.8X", generateRnd32());
	printf("New ChargePointID: %s\n", settings->ChargePointId);
	if(Settings_save())
		printf("Setting saving is success\n");
	else
		printf("Setting saving is failed\n");
}

void testReconnect(){
	NetInputMessage mes;
	mes.messageId = NET_INPUT_MESSAGE_RECONNECT;
	NET_sendInputMessage(&mes);
}

void printCurrentDateTime(){
	struct tm dt;
	
	if(isNeedSetTime){
		dt.tm_mday = 11;
		dt.tm_mon = 7;
		dt.tm_year = 2018;
	
		dt.tm_hour = 15;
		dt.tm_min = 41;
		dt.tm_sec = 39;
	
		setCurrentTime(&dt);
		
		isNeedSetTime = false;
	}
	else{
		getCurrentTime(&dt);
		printf("DateTime: %.2d.%.2d.%.4d %.2d:%.2d:%.2d\n", dt.tm_mday, dt.tm_mon, dt.tm_year, dt.tm_hour, dt.tm_min, dt.tm_sec);
	}
}

void updateMeterValues(void){
	int i;
	ChargePointConnector *conn;
	
	for(i = 0; i < CONFIGURATION_NUMBER_OF_CONNECTORS; i++){
			conn = &connector[i];
			if(conn->status == CHARGE_POINT_STATUS_CHARGING){
				conn->isMeterValueRequest = true;
			}
	}
}

void mainDispatcher(void){
	uint32_t currentTick, lastLedTick;
	uint32_t lastSendTick, lastMeterReqTick;
	char sendData[16];
	GeneralMessage message;
	uint32_t btnPressCnt = 0;
	
	ChargePoint_init();
	
	Settings_init();
	settings = Settings_get();
	printf("ChargePointID: %s\n", settings->ChargePointId);
	
	localAuthList_load();
	
	fillOcppConfigurationWithDefValues(&ocppConfVaried, &ocppConfFixed, &ocppConfRestrict);
	
	
	
	initDisplay();
	initConnectors();
	checkHardware();
	
	Display_clear();
	Indication_Init(connector);
	
	if(ChargePoint_getStatusState() != CHARGE_POINT_STATUS_FAULTED){
		ChargePoint_setStatusState(CHARGE_POINT_STATUS_AVAILABLE, 0, NULL);
	}
	
	Channel_start(TASK_TAG_CHANNELS, mainQueue, connector, CONFIGURATION_NUMBER_OF_CONNECTORS);
	
	RFID_start(TASK_TAG_RFID, mainQueue);
	SerialControl_start(TASK_TAG_SERIAL_CONTROL, mainQueue);
	NET_start(TASK_TAG_NET, mainQueue);
	
	Display_PrintStrCenter(0, "Waiting\0");
	
	lastLedTick = HAL_GetTick(); 
	lastSendTick = lastLedTick;
	lastMeterReqTick = lastLedTick;
	
	// Infinite loop 
  for(;;)
  {
		
		while(xQueueReceive(mainQueue, &message, 0) == pdPASS){
			switch(message.sourceTag){
				case TASK_TAG_RFID:
					processMessageFromRFID(&message);
					break;
				case TASK_TAG_USER_BUTTON:
					//printf("User button is pressed %d\n", ++btnPressCnt);
				  //testSaveSetting();
				  //printCurrentDateTime();
				  NET_test();
				  //testReconnect();
					break;
				case TASK_TAG_SERIAL_CONTROL:
					processMessageFromSerialControl(&message);
					break;
				case TASK_TAG_NET:
					processMessageFromNET(&message);
					break;
				case TASK_TAG_CHANNELS:
					processMessageFromChannels(&message);
					break;
			}
			
		}
		
		currentTick = HAL_GetTick();
		if((currentTick - lastLedTick) >= 500){
			lastLedTick = currentTick;
			toggleBlueLed();
		}
		
		if((currentTick - lastSendTick) >= 1000){
			lastSendTick = currentTick;
			sprintf(sendData, "%.8X\r", lastSendTick);
			//CDC_Transmit_FS((uint8_t*)sendData, strlen(sendData));
		}
		
		//For channel with charging process need update meter value
		if((currentTick - lastMeterReqTick) >= 1000){
			lastMeterReqTick = currentTick;
			updateMeterValues();
			
		}
		
		Indication_CheckMessage();
		osDelay(1);
	}
}

void onUserButtonPressed(){
	GeneralMessage message;
	uint32_t tick;
	tick = HAL_GetTick();
	if((tick - lastUserButtonPressTick) >= 200){
		lastUserButtonPressTick = tick;
		message.sourceTag = TASK_TAG_USER_BUTTON;
		//This function is called from Interrupt so use ISR version of xQueueSend
		xQueueSendFromISR(mainQueue, &message, NULL);
	}
	
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	switch(GPIO_Pin){
		case USER_Btn_Pin:
			onUserButtonPressed();
			break;
	}
}

/* USER CODE END 4 */

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();

  /* init code for LWIP */
  //MX_LWIP_Init();

  /* USER CODE BEGIN 5 */
	mainDispatcher();
	
	
  /* Infinite loop */
	/// !!! Сюда мы не должны доходить
  for(;;)
  {
		//RFID_check_connection();
		//HAL_Delay(100);
		/*if(MFRC522_Check(str) == MI_OK){
			printf("Found card 0x%.2X%.2X%.2X%.2X\n", str[0], str[1], str[2], str[3]);
			HAL_Delay(100);
		}*/
		
    osDelay(1);
  }
  /* USER CODE END 5 */ 
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
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
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
