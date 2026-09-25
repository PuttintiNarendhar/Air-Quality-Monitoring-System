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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdlib.h>
#include "queue.h"
#include "semphr.h" // Required for Mutex
#include <math.h>
#include <stdio.h>
#include "hts221.h"
#include "lps22hb.h" // Assuming these provide register definitions
#include "fonts.h"
#include "st7735.h"
#include "st7735_cfg.h"




/* USER CODE BEGIN Includes */
// ... existing includes ...
//#include "sgp40.h"
//#include "PMS5003_HAL_STM32.h"
//#include "sensirion_gas_index_algorithm.h" // Might not be needed in main.c, only in driver.c
/* USER CODE END Includes */
#include <stdbool.h>
#include "PMS5003.h"
#include "stm32l475e_iot01_tsensor.h"
#include "driver_sgp30.h"
#include "PMS5003_HAL_STM32.h"
#include "stm32l475e_iot01.h"

PMS_typedef pms;

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define HTS221_ADDR  (0x5F << 1)
//#define LPS22HB_ADDR  (0x5C << 1)
#define LPS22HB_ADDR (0x5D << 1)
#define I2C_TIMEOUT_MS     100

#define PM25_GOOD_MAX       12    // µg/m³
#define PM25_MODERATE_MAX   35

#define MAX(a,b) ((a) > (b) ? (a) : (b))


//static uint8_t sgp30_i2c_write(uint8_t addr, uint8_t *buf, uint16_t len) {
//    return (HAL_I2C_Master_Transmit(&hi2c1, addr, buf, len, 100) == HAL_OK) ? 0 : 1;
//}
//
//static uint8_t sgp30_i2c_read(uint8_t addr, uint8_t *buf, uint16_t len) {
//    return (HAL_I2C_Master_Receive(&hi2c1, addr, buf, len, 100) == HAL_OK) ? 0 : 1;
//}

static void sgp30_delay_ms(uint32_t ms) {
    HAL_Delay(ms);
}


TaskHandle_t pSOS_Transmit;
TickType_t LastWakeTime;
TaskHandle_t pUART_SendMsg;
uint8_t repeat;
uint8_t cmd;
QueueHandle_t pUARTQueue;
uint8_t receiveBuf;
TaskHandle_t pTempSensorRead;
TaskHandle_t pLCDDisplay;
TaskHandle_t pHumidPressRead;
TaskHandle_t pSensorReadTask;
QueueHandle_t pmsRxQueue = NULL;

sgp30_handle_t sgp30_handle;   // global handle for the sensor
// Shared data protected by the Mutex
SemaphoreHandle_t sensorDataMutex;
volatile float currentTemperature = 0.0f;
volatile float currentHumidity = 0.0f;
volatile float currentPressure = 0.0f; // in hPa

/* USER CODE BEGIN PV */
// ... existing variables ...
//volatile int32_t currentVOCIndex = 0; // SGP40 processed index
volatile uint16_t currentTVOC = 0;   // SGP30 Total Volatile Organic Compounds (ppb)
volatile uint16_t currentCO2eq = 0;  // SGP30 CO2 equivalent (ppm)


volatile uint16_t currentPM2_5 = 0;  // PMS5003 PM2.5 value
volatile uint16_t currentPM10 = 0;   // PMS5003 PM10 value
uint8_t pms5003_rx_byte;  // holds 1 byte received from PMS5003

/* --- SGP30 status flag (FILE SCOPE) --- */
static uint32_t sgp30_seconds = 0;
static uint8_t sgp30_ready = 0;


/* USER CODE END PV */
/* USER CODE BEGIN 0 */

// --- SGP30 HAL Wrappers ---

/* USER CODE END 0 */

/*
int readComplete = 0;
int writeComplete = 0;
*/

//char msg[] = "=====> Temperature sensor HT221 initialized \r\n";
char msg[] = "=====> System Initialized \r\n";
/*
char msg1[] = "Hello from RXCallback\r\n";
char msg2[] = "Hello from TXCallback\r\n";
*/



/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

DFSDM_Channel_HandleTypeDef hdfsdm1_channel2;

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

OSPI_HandleTypeDef hospi1;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi3;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
UART_HandleTypeDef PMS5003_UART_HANDLE;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_DFSDM1_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C2_Init(void);
static void MX_OCTOSPI1_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI3_Init(void);
static void MX_UART4_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USB_OTG_FS_USB_Init(void);
/* USER CODE BEGIN PFP */
static void MyLED2_init(void);
//static void MyUART1_init(void);
//static void ST7735_GPIO_Init(void);
static void SOS_Transmit(void * parameter);
//static void UART_SendMsg(void * parameter);
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
//static void TempSensorRead(void * parameter);
static void SensorReadTask(void * parameter);
//static void MyTSENSOR_init();
//static float MyTSENSOR_ReadTemp();
static void LCDDisplay(void * parameter);
//static void HumidPressRead(void * parameter);
static void SerialMonitorTask(void *parameter);

// Sensor Helper Prototypes
static void Init_HTS221();
static void Init_LPS22HB();
static float Read_HTS221_Temperature();
static float Read_HTS221_Humidity();
static float Read_LPS22HB_Pressure();

static uint16_t AQI_Linear(float C,
                           float Clow,
                           float Chigh,
                           uint16_t Ilow,
                           uint16_t Ihigh);

static uint16_t AQI_PM25(float pm25);
static uint16_t AQI_CO2(uint16_t co2);
static uint16_t AQI_TVOC(uint16_t tvoc);
uint16_t Calculate_AQI(void);


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint16_t Calculate_AQI(void)
{
    uint16_t aqi_pm25 = AQI_PM25((float)currentPM2_5);
    uint16_t aqi_co2  = AQI_CO2(currentCO2eq);
    uint16_t aqi_tvoc = AQI_TVOC(currentTVOC);

    return MAX(aqi_pm25, MAX(aqi_co2, aqi_tvoc));
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	BaseType_t flag;
	//BaseType_t flag_uart;
	//BaseType_t flag_tempSensor;
	BaseType_t flag_SensorReadTask;
	BaseType_t flag_lcd;
	BaseType_t flag_serial;
	//char msg_lcd2[] = "Hello World - Display on SPI TFT LCD!\n";
	char msg_lcd2[] = "AirQuality Monitoring System\n";
	//BaseType_t flag_HumidPressSensor;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_DFSDM1_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_OCTOSPI1_Init();
  MX_SPI1_Init();
  MX_SPI3_Init();
  MX_UART4_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_USB_Init();
  /* USER CODE BEGIN 2 */
  MyLED2_init();

  //BSP_TSENSOR_Init();
  // ST7735_GPIO_Init() must be called before ST7735_Init()
  ST7735_Init();
  ST7735_Backlight_On();
  ST7735_FillScreen(ST7735_RED);

  BSP_LED_Init(LED2);


  //ST7735_DrawString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor);
  ST7735_DrawString(0, 0, msg_lcd2, Font_7x10, ST7735_WHITE, ST7735_BLACK);

  HAL_UART_Transmit(&huart2, (uint8_t *) msg, strlen(msg), 1000);

  // Create the Mutex before starting the tasks, Initialize the I2C Bus Mutex
  sensorDataMutex = xSemaphoreCreateMutex();
  if (sensorDataMutex == NULL) {
        Error_Handler(); // System failure if we can't create the mutex
    }

  // Initialize SGP40 algorithm and start I2C communications
//  driver_sgp30_init();// Initialize SGP30 sensor
//  driver_sgp30_iaq_init(); // Start IAQ baseline algorithm
  // Link HAL functions to the driver handle
//  sgp30_handle.i2c_write = sgp30_i2c_write;
//  sgp30_handle.i2c_read  = sgp30_i2c_read;
  sgp30_handle.delay_ms  = sgp30_delay_ms;
  sgp30_handle.debug_print = NULL;   // optional
  // --- Initialize SGP30 ---
/*     if (sgp30_init() != 0) {
         Error_Handler();   // handle init failure
     }
     if (sgp30_iaq_init() != 0) {
         Error_Handler();   // handle IAQ baseline init failure
     }*/
  // --- Initialize SGP30 ---
 /* if (sgp30_init(&sgp30_handle) != 0) {
      Error_Handler();   // handle init failure
  }

  if (sgp30_iaq_init(&sgp30_handle) != 0) {
      Error_Handler();   // handle IAQ baseline init failure
  }*/

  // Start continuous UART reception for PMS5003
  //PMS5003_StartReception();
  //HAL_UART_Receive_IT(&huart4, &pms5003_rx_byte, 1);

  // Create Tasks
  /*flag = xTaskCreate(SOS_Transmit, "SOS_Transmit", 512, (void*) 300, 3, &pSOS_Transmit);
  if(flag != pdPASS)
  {
	  Error_Handler();
  }*/

  //LastWakeTime = xTaskGetTickCount();


/*
  flag_uart = xTaskCreate(UART_SendMsg, "UART_SendMsg", 200, "\r\nSend SOS with LEDs!!! Please enter 1, 2, or 3 for the number of repeats: ", 2, &pUART_SendMsg);
  if(flag_uart != pdPASS)
    {
  	  Error_Handler();
    }

  HAL_UART_Receive_IT(&huart1, &receiveBuf, 1);

  pUARTQueue = xQueueCreate(10, sizeof(uint8_t));
*/

  /*flag_tempSensor = xTaskCreate(TempSensorRead, "TempSensorRead", 1024, "\r\nTemperature Sensor Reading!!!\r\n", 3, &pTempSensorRead);
  if(flag_tempSensor != pdPASS)
    {
  	  Error_Handler();
    }*/


  	  /*flag_SensorReadTask = xTaskCreate(SensorReadTask, "SensorReadTask", configMINIMAL_STACK_SIZE * 3, NULL, 3, &pSensorReadTask);
    if(flag_SensorReadTask != pdPASS)
      {
    	  Error_Handler();
      }*/
    flag_SensorReadTask = xTaskCreate(SensorReadTask, "SensorReadTask", 1024, NULL, 3, &pSensorReadTask);
        if(flag_SensorReadTask != pdPASS)
          {
        	  Error_Handler();
          }

  // Create Humidity and Pressure Sensor task (Priority 3)
  // This task will call the separate functions internally
/*
  flag_HumidPressSensor = xTaskCreate(HumidPressRead, "HumidPressRead", 1024, "\r\nHumidpress Sensor Reading!!!\r\n", 3, &pHumidPressRead);
    if(flag_HumidPressSensor != pdPASS)
      {
    	  Error_Handler();
      }
*/


  /*flag_lcd = xTaskCreate(LCDDisplay, "LCDDisplay", configMINIMAL_STACK_SIZE * 3, NULL, 1, &pLCDDisplay); // Lower priority than sensors
  if(flag_lcd != pdPASS)
  {
	  Error_Handler();
  }*/

  xTaskCreate(LCDDisplay, "LCD", 768, NULL, 1, NULL);


  /*flag_serial = xTaskCreate(SerialMonitorTask, "SerialMonitorTask", configMINIMAL_STACK_SIZE * 3, NULL, 1, NULL);
  if(flag_serial != pdPASS)
  {
   	  Error_Handler();
  }*/
  pmsRxQueue = xQueueCreate(128, sizeof(uint8_t));
  if (pmsRxQueue == NULL)
  {
      Error_Handler();  // Queue creation failed
  }

  //Start Scheduler
  vTaskStartScheduler();

  /* USER CODE END 2 */

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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE
                              |RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV8;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB|RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 24;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_48M2CLK|RCC_PLLSAI1_ADC1CLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief DFSDM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_DFSDM1_Init(void)
{

  /* USER CODE BEGIN DFSDM1_Init 0 */

  /* USER CODE END DFSDM1_Init 0 */

  /* USER CODE BEGIN DFSDM1_Init 1 */

  /* USER CODE END DFSDM1_Init 1 */
  hdfsdm1_channel2.Instance = DFSDM1_Channel2;
  hdfsdm1_channel2.Init.OutputClock.Activation = ENABLE;
  hdfsdm1_channel2.Init.OutputClock.Selection = DFSDM_CHANNEL_OUTPUT_CLOCK_SYSTEM;
  hdfsdm1_channel2.Init.OutputClock.Divider = 2;
  hdfsdm1_channel2.Init.Input.Multiplexer = DFSDM_CHANNEL_EXTERNAL_INPUTS;
  hdfsdm1_channel2.Init.Input.DataPacking = DFSDM_CHANNEL_STANDARD_MODE;
  hdfsdm1_channel2.Init.Input.Pins = DFSDM_CHANNEL_SAME_CHANNEL_PINS;
  hdfsdm1_channel2.Init.SerialInterface.Type = DFSDM_CHANNEL_SPI_RISING;
  hdfsdm1_channel2.Init.SerialInterface.SpiClock = DFSDM_CHANNEL_SPI_CLOCK_INTERNAL;
  hdfsdm1_channel2.Init.Awd.FilterOrder = DFSDM_CHANNEL_FASTSINC_ORDER;
  hdfsdm1_channel2.Init.Awd.Oversampling = 1;
  hdfsdm1_channel2.Init.Offset = 0;
  hdfsdm1_channel2.Init.RightBitShift = 0x00;
  if (HAL_DFSDM_ChannelInit(&hdfsdm1_channel2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DFSDM1_Init 2 */

  /* USER CODE END DFSDM1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00100D14;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x00100D14;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief OCTOSPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_OCTOSPI1_Init(void)
{

  /* USER CODE BEGIN OCTOSPI1_Init 0 */

  /* USER CODE END OCTOSPI1_Init 0 */

  OSPIM_CfgTypeDef OSPIM_Cfg_Struct = {0};

  /* USER CODE BEGIN OCTOSPI1_Init 1 */

  /* USER CODE END OCTOSPI1_Init 1 */
  /* OCTOSPI1 parameter configuration*/
  hospi1.Instance = OCTOSPI1;
  hospi1.Init.FifoThreshold = 1;
  hospi1.Init.DualQuad = HAL_OSPI_DUALQUAD_DISABLE;
  hospi1.Init.MemoryType = HAL_OSPI_MEMTYPE_MACRONIX;
  hospi1.Init.DeviceSize = 32;
  hospi1.Init.ChipSelectHighTime = 1;
  hospi1.Init.FreeRunningClock = HAL_OSPI_FREERUNCLK_DISABLE;
  hospi1.Init.ClockMode = HAL_OSPI_CLOCK_MODE_0;
  hospi1.Init.ClockPrescaler = 1;
  hospi1.Init.SampleShifting = HAL_OSPI_SAMPLE_SHIFTING_NONE;
  hospi1.Init.DelayHoldQuarterCycle = HAL_OSPI_DHQC_DISABLE;
  hospi1.Init.ChipSelectBoundary = 0;
  hospi1.Init.DelayBlockBypass = HAL_OSPI_DELAY_BLOCK_BYPASSED;
  if (HAL_OSPI_Init(&hospi1) != HAL_OK)
  {
    Error_Handler();
  }
  OSPIM_Cfg_Struct.ClkPort = 1;
  OSPIM_Cfg_Struct.NCSPort = 1;
  OSPIM_Cfg_Struct.IOLowPort = HAL_OSPIM_IOPORT_1_LOW;
  if (HAL_OSPIM_Config(&hospi1, &OSPIM_Cfg_Struct, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN OCTOSPI1_Init 2 */

  /* USER CODE END OCTOSPI1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  //hspi3.Init.DataSize = SPI_DATASIZE_4BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 7;
  hspi3.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi3.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  //huart4.Init.BaudRate = 115200;
  huart4.Init.BaudRate = 9600;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart4, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart4, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}



/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief USB_OTG_FS Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_OTG_FS_USB_Init(void)
{

  /* USER CODE BEGIN USB_OTG_FS_Init 0 */

  /* USER CODE END USB_OTG_FS_Init 0 */

  /* USER CODE BEGIN USB_OTG_FS_Init 1 */

  /* USER CODE END USB_OTG_FS_Init 1 */
  /* USER CODE BEGIN USB_OTG_FS_Init 2 */

  /* USER CODE END USB_OTG_FS_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, ST25DV04K_RF_DISABLE_Pin|ISM43362_RST_Pin|ISM43362_SPI3_CSN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, ARD_D10_Pin|ARD_D4_Pin|ARD_D7_Pin|SPBTLE_RF_RST_Pin
                          |ARD_D9_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, ARD_D8_Pin|ISM43362_BOOT0_Pin|ISM43362_WAKEUP_Pin|LED2_Pin
                          |SPSGRF_915_SDN_Pin|ARD_D5_Pin|SPSGRF_915_SPI3_CSN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, SPBTLE_RF_SPI3_CSN_Pin|PMOD_RESET_Pin|PMOD_SPI2_SCK_Pin|STSAFE_A110_RESET_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, VL53L0X_XSHUT_Pin|LED3_WIFI__LED4_BLE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : ST25DV04K_RF_DISABLE_Pin ISM43362_RST_Pin ISM43362_SPI3_CSN_Pin */
  GPIO_InitStruct.Pin = ST25DV04K_RF_DISABLE_Pin|ISM43362_RST_Pin|ISM43362_SPI3_CSN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : USB_OTG_FS_OVRCR_EXTI3_Pin ST25DV04K_GPO_Pin SPSGRF_915_GPIO3_EXTI5_Pin SPBTLE_RF_IRQ_EXTI6_Pin
                           ISM43362_DRDY_EXTI1_Pin */
  GPIO_InitStruct.Pin = USB_OTG_FS_OVRCR_EXTI3_Pin|ST25DV04K_GPO_Pin|SPSGRF_915_GPIO3_EXTI5_Pin|SPBTLE_RF_IRQ_EXTI6_Pin
                          |ISM43362_DRDY_EXTI1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : BUTTON_EXTI13_Pin VL53L0X_GPIO1_EXTI7_Pin LSM3MDL_DRDY_EXTI8_Pin */
  GPIO_InitStruct.Pin = BUTTON_EXTI13_Pin|VL53L0X_GPIO1_EXTI7_Pin|LSM3MDL_DRDY_EXTI8_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : ARD_D10_Pin ARD_D4_Pin ARD_D7_Pin SPBTLE_RF_RST_Pin
                           ARD_D9_Pin */
  GPIO_InitStruct.Pin = ARD_D10_Pin|ARD_D4_Pin|ARD_D7_Pin|SPBTLE_RF_RST_Pin
                          |ARD_D9_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : ARD_D3_Pin */
  GPIO_InitStruct.Pin = ARD_D3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ARD_D3_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : ARD_D6_Pin */
  GPIO_InitStruct.Pin = ARD_D6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
  HAL_GPIO_Init(ARD_D6_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : ARD_D8_Pin ISM43362_BOOT0_Pin ISM43362_WAKEUP_Pin SPSGRF_915_SDN_Pin
                           ARD_D5_Pin SPSGRF_915_SPI3_CSN_Pin */
  GPIO_InitStruct.Pin = ARD_D8_Pin|ISM43362_BOOT0_Pin|ISM43362_WAKEUP_Pin|SPSGRF_915_SDN_Pin
                          |ARD_D5_Pin|SPSGRF_915_SPI3_CSN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : LED2_Pin */
  GPIO_InitStruct.Pin = LED2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(LED2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LPS22HB_INT_DRDY_EXTI10_Pin LSM6DSL_INT1_EXTI11_Pin USB_OTG_FS_PWR_EN_Pin ARD_D2_Pin
                           HTS221_DRDY_EXTI15_Pin PMOD_IRQ_EXTI2_Pin */
  GPIO_InitStruct.Pin = LPS22HB_INT_DRDY_EXTI10_Pin|LSM6DSL_INT1_EXTI11_Pin|USB_OTG_FS_PWR_EN_Pin|ARD_D2_Pin
                          |HTS221_DRDY_EXTI15_Pin|PMOD_IRQ_EXTI2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : SPBTLE_RF_SPI3_CSN_Pin PMOD_RESET_Pin PMOD_SPI2_SCK_Pin STSAFE_A110_RESET_Pin */
  GPIO_InitStruct.Pin = SPBTLE_RF_SPI3_CSN_Pin|PMOD_RESET_Pin|PMOD_SPI2_SCK_Pin|STSAFE_A110_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : VL53L0X_XSHUT_Pin LED3_WIFI__LED4_BLE_Pin */
  GPIO_InitStruct.Pin = VL53L0X_XSHUT_Pin|LED3_WIFI__LED4_BLE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OTG_FS_VBUS_Pin */
  GPIO_InitStruct.Pin = USB_OTG_FS_VBUS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OTG_FS_VBUS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : USB_OTG_FS_ID_Pin USB_OTG_FS_DM_Pin USB_OTG_FS_DP_Pin */
  GPIO_InitStruct.Pin = USB_OTG_FS_ID_Pin|USB_OTG_FS_DM_Pin|USB_OTG_FS_DP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/*Configure GPIO pin : LED2_Pin */
/*
  GPIO_InitStruct.Pin = LED2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(LED2_GPIO_Port, &GPIO_InitStruct);
*/


static void MyLED2_init(void)
{
	//Configure the MODE register
	//01: general purpose output
	//MODER Bit 29 and Bit 28
	//GPIOB->MODER &= ~ (1 << 29);
	//GPIOB->MODER |= (1 << 28);
	//RESET both bits 29 and 28 to 0
	GPIOB->MODER &= ~ (0x3 << 28);
	//SET the values
	GPIOB->MODER |= (0x1 << 28);

	//Configure the Output Type Register
	// 0: Push-Pull
	//OTYPER Bit 14
	GPIOB->OTYPER &= ~ (1 << 14);

	//Configure the Speed Register
	//11: Very High Speed
	//OSPEEDR Bit 29 and Bit 28

	//RESET both bits 29 and 28 to 0
	GPIOB->OSPEEDR &= ~ (0x3 << 28);
	//SET the values
	GPIOB->OSPEEDR |= (0x3 << 28);

	//Configure pull-up/pull-down register
	// 00: No pull-up/pull-down
	//PUPDR Bit 29 and Bit 28
	//RESET both bits 29 and 28 to 0
	GPIOB->PUPDR &= ~ (0x3 << 28);
	//SET the values
	GPIOB->PUPDR |= (0x0 << 28);


}
/*
static void MyButton_init(void)
{
	//Button PortC Pin 13, EXTI13

	//Configure SYSCLK
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	//Configure the MODE register
	//input mode 00 for interrupt
	//00: input

	//MODER Bit 27 and Bit 26
	//RESET these bits to 00

	GPIOC->MODER &= ~ (0x3 << 26);



	//Configure pull-up/pull-down register
	// 00: No pull-up/pull-down
	//PUPDR Bit xx and Bit xx
	//RESET both bits xx and xx to 0
	GPIOC->PUPDR &= ~ (0x3 << 26 );
	//SET the values
	//GPIOC->PUPDR |= (0x0 << 26);

	//EXTI13, Choose Port C

	//Configure the External Interrupt Configuration Register
	//SYSCFG_EXTICR4
	SYSCFG->EXTICR[3] &= ~(0xF << 4); // Clear EXTI13[7:4]
	SYSCFG->EXTICR[3] |= (0x2 << 4); // Set to 0x2 for Port C

	//Configure the Falling Trigger Selection Register
	//EXTI_FTSR
	EXTI->FTSR1 |= (1 << 13);

	//EXTI Line Configuration
	EXTI->EMR1 &= ~(1 << 13); //disable event mask register because we only care about interrupt

	//Configure the Interrupt Mask Register
	//EXTI_IMR
	EXTI->IMR1 |= (1 << 13);

	//Configure NVIC Set-Enable Register
	//NVIC_ISER
	//ISER[] is not a read-modify-write register. It's a write-one-to-set register.
	NVIC->ISER[EXTI15_10_IRQn / 32] = (1 << (EXTI15_10_IRQn % 32));
	//NVIC_EnableIRQ(EXTI15_10_IRQn);



}
*/

/*
static void MyUART1_init(void)
{
	//configure UART1 manually excluding Baud Rate Register

	//configurations for M Bits, Parity Enable, ...
	//CR1, CR2
	//configure M bits
	//00 : 1 start bit, 8 data bits, n stop bits
	//M1 : Bit 28
	//huart1.Instance->CR1

	//M0 : Bit 12
	//huart1.Instance->CR1



}
*/

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

	xTaskNotifyFromISR(pUART_SendMsg, 0, eNoAction, NULL);


}

static void SOS_Transmit(void * parameter)
{
	repeat = 2;
	while(1)
	{
	  //activate when the user enters the number of repeats

	  //for(int j=0; j<3; j++)
	  //while(repeat > 0)
	  while(1)
	  {
	  // transmit SOS three times


	  //sending 'S'
	  for(int i=0; i<3; i++)
	  {
		  //light up the LED for a DOT
		  //write a 1 to Bit 14 of GPIIOB ODR (Output Data Register)
		  //HAL_GPIO_WritePin(GPIOB, LED2_Pin, GPIO_PIN_SET);
		  GPIOB->ODR |= (1 << 14);

		  //HAL_Delay(300);
		  vTaskDelay(300);


		  //turn off the LED for spacing
		  //HAL_GPIO_WritePin(GPIOB, LED2_Pin, GPIO_PIN_RESET);
		  //write a 0 to Bit 14 of GPIIOB ODR (Output Data Register)
		  GPIOB->ODR &= ~ (1 << 14);

		  //HAL_Delay(300);
		  vTaskDelay(300);
	  }
	  // THREE TIME UNITS BETWEEN CHARACTERS (two additional units)
	  //HAL_Delay(300*2);
	  vTaskDelay(300*2);

	  //sending 'O'
	  	  for(int i=0; i<3; i++)
	  	  {
	  		  //light up the LED for a DASH
	  		  //HAL_GPIO_WritePin(GPIOB, LED2_Pin, GPIO_PIN_SET);

	  		  //write a 1 to Bit 14 of GPIIOB ODR (Output Data Register)
		  	  //HAL_GPIO_WritePin(GPIOB, LED2_Pin, GPIO_PIN_SET);
	  		  GPIOB->ODR |= (1 << 14);

	  		  //HAL_Delay(300*3);
	  		  vTaskDelay(300*3);

	  		  //turn off the LED for spacing
	  		  //HAL_GPIO_WritePin(GPIOB, LED2_Pin, GPIO_PIN_RESET);
	     	  //write a 0 to Bit 14 of GPIIOB ODR (Output Data Register)
	  		  GPIOB->ODR &= ~ (1 << 14);

	  		  //HAL_Delay(300);
	  		  vTaskDelay(300);
	  	  }
	  	  // THREE TIME UNITS BETWEEN CHARACTERS (two additional units)
	  	  //HAL_Delay(300*2);
	  	  vTaskDelay(300*2);

	  	//sending 'S'
	  		  for(int i=0; i<3; i++)
	  		  {
	  			//light up the LED for a DOT
	  			//write a 1 to Bit 14 of GPIIOB ODR (Output Data Register)
	  			//HAL_GPIO_WritePin(GPIOB, LED2_Pin, GPIO_PIN_SET);
	  			GPIOB->ODR |= (1 << 14);

	  			//HAL_Delay(300);
	  			vTaskDelay(300);
	  			//turn off the LED for spacing
	  		    //HAL_GPIO_WritePin(GPIOB, LED2_Pin, GPIO_PIN_RESET);
	     		//write a 0 to Bit 14 of GPIIOB ODR (Output Data Register)
	  			GPIOB->ODR &= ~ (1 << 14);

	     		//HAL_Delay(300);
	  			vTaskDelay(300);
	  		  }
	  		  // SEVEN TIME UNITS BETWEEN WORDS (six additional units)
	  		  //HAL_Delay(300*6);
	  		  vTaskDelay(300*6);

	  repeat--;
	  }
	  //SOS_transmission = 0;

	  xQueueReceive(pUARTQueue, &cmd, portMAX_DELAY);

	  if(cmd - '0' >=1 && cmd - '0' <=3)
	  {
		  repeat = cmd - '0';
	  }
	  else
	  {
		  repeat = 0;
	  }
	}





}

// --- Sensor Helper Implementations using Blocking I2C + Mutex ---

static void Init_HTS221() {
    uint8_t ctrl1_reg_val = 0;

        ctrl1_reg_val |= (1 << 2); // BDU
        ctrl1_reg_val |= (1 << 0); // ODR 1Hz
        ctrl1_reg_val |= (1 << 7); // PD (Power Down bit reset = Active)
        HAL_I2C_Mem_Write(&hi2c2, HTS221_ADDR, 0x20, I2C_MEMADD_SIZE_8BIT, &ctrl1_reg_val, 1, I2C_TIMEOUT_MS);

}

static void Init_LPS22HB() {
    /*uint8_t ctrl1_reg_val = 0;

        ctrl1_reg_val |= (1 << 2); // BDU
        ctrl1_reg_val |= (1 << 0); // ODR 1Hz
        ctrl1_reg_val |= (1 << 7); // PD
        HAL_I2C_Mem_Write(&hi2c2, LPS22HB_ADDR, 0x10, I2C_MEMADD_SIZE_8BIT, &ctrl1_reg_val, 1, I2C_TIMEOUT_MS);*/

	/*uint8_t ctrl1 = 0x50; // ODR = 10 Hz
	HAL_I2C_Mem_Write(&hi2c2,
	                  LPS22HB_ADDR,
	                  0x10,
	                  I2C_MEMADD_SIZE_8BIT,
	                  &ctrl1, 1, 10);*/
	uint8_t ctrl1 =
	        (1 << 7) |   // PD = 1 → power ON
	        (2 << 4) |   // ODR = 010 → 10 Hz
	        (1 << 1);    // BDU = 1

	    // ctrl1 = 0xA2

	   /* HAL_I2C_Mem_Write(&hi2c2,
	                      LPS22HB_ADDR,
	                      0x10,                 // CTRL_REG1
	                      I2C_MEMADD_SIZE_8BIT,
	                      &ctrl1,
	                      1,
	                      10);//WHO_AM_I read test*/


	    /* ---- UART print (METHOD 3) ---- */
	    /*char msg[32];
	    sprintf(msg, "LPS22HB WHOAMI=0x%02X\r\n", whoami);
	    HAL_UART_Transmit(&huart1,
	                      (uint8_t *)msg,
	                      strlen(msg),
	                      100);*/


}

static float Read_HTS221_Temperature() {
	int16_t T0_out, T1_out, T_out;
	    int16_t T0_degC, T1_degC;
	    uint8_t buffer[4], reg_val;
	    float temperature = 0.0f;

	    // Acquire I2C Bus Mutex


	        // 1. Read calibration data (T0_degC_x8 and T1_degC_x8)
	        if(HAL_I2C_Mem_Read(&hi2c2, HTS221_ADDR, 0x32 | 0x80, I2C_MEMADD_SIZE_8BIT, buffer, 2, I2C_TIMEOUT_MS)!=HAL_OK)
	        {
	                return NAN;   // ❌ I2C failed → do not block task
	        }
	        if(HAL_I2C_Mem_Read(&hi2c2, HTS221_ADDR, 0x35, I2C_MEMADD_SIZE_8BIT, &reg_val, 1, I2C_TIMEOUT_MS)!=HAL_OK)
	        {
	        	                return NAN;   // ❌ I2C failed → do not block task
	        	        }
	        uint16_t T0_degC_x8_u16 = (((uint16_t)(reg_val & 0x03)) << 8) | ((uint16_t)buffer[0]);
	        uint16_t T1_degC_x8_u16 = (((uint16_t)(reg_val & 0x0C)) << 6) | ((uint16_t)buffer[1]);
	        T0_degC = T0_degC_x8_u16 >> 3; // Divide by 8
	        T1_degC = T1_degC_x8_u16 >> 3; // Divide by 8

	        // 2. Read calibration data (T0_out and T1_out)
	        if(HAL_I2C_Mem_Read(&hi2c2, HTS221_ADDR, 0x3C | 0x80, I2C_MEMADD_SIZE_8BIT, buffer, 4, I2C_TIMEOUT_MS)!=HAL_OK)
	        {
	        	                return NAN;   // ❌ I2C failed → do not block task
	        	        }
	        T0_out = (((int16_t)buffer[1]) << 8) | (int16_t)buffer[0];
	        T1_out = (((int16_t)buffer[3]) << 8) | (int16_t)buffer[2];

	        // 3. Read current temperature raw data
	        if(HAL_I2C_Mem_Read(&hi2c2, HTS221_ADDR, 0x2A | 0x80, I2C_MEMADD_SIZE_8BIT, buffer, 2, I2C_TIMEOUT_MS)!=HAL_OK)
	        {
	        	                return NAN;   // ❌ I2C failed → do not block task
	        	        }
	        T_out = (((int16_t)buffer[1]) << 8) | (int16_t)buffer[0];

	        // 4. Linear interpolation (Datasheet TN1218 formula)
	        temperature = (float)(T_out - T0_out) * (float)(T1_degC - T0_degC) / (float)(T1_out - T0_out) + (float)T0_degC;

	    return temperature;
}

static float Read_HTS221_Humidity()
{
    uint8_t buffer[2];
    uint8_t H0_x2, H1_x2;
    int16_t H0_T0_OUT, H1_T0_OUT;
    int16_t H_OUT;

    // 1. Read H0_rH_x2 and H1_rH_x2
    HAL_I2C_Mem_Read(&hi2c2, HTS221_ADDR, 0x30, I2C_MEMADD_SIZE_8BIT, &H0_x2, 1, 10);
    HAL_I2C_Mem_Read(&hi2c2, HTS221_ADDR, 0x31, I2C_MEMADD_SIZE_8BIT, &H1_x2, 1, 10);

    float H0_rH = H0_x2 / 2.0f;
    float H1_rH = H1_x2 / 2.0f;

    // 2. Read calibration output values
    HAL_I2C_Mem_Read(&hi2c2, HTS221_ADDR, 0x36 | 0x80, I2C_MEMADD_SIZE_8BIT, buffer, 2, 10);
    H0_T0_OUT = (int16_t)((buffer[1] << 8) | buffer[0]);

    HAL_I2C_Mem_Read(&hi2c2, HTS221_ADDR, 0x3A | 0x80, I2C_MEMADD_SIZE_8BIT, buffer, 2, 10);
    H1_T0_OUT = (int16_t)((buffer[1] << 8) | buffer[0]);

    // 3. Read current humidity output
    HAL_I2C_Mem_Read(&hi2c2, HTS221_ADDR, 0x28 | 0x80, I2C_MEMADD_SIZE_8BIT, buffer, 2, 10);
    H_OUT = (int16_t)((buffer[1] << 8) | buffer[0]);

    // 4. Linear interpolation
    float humidity = (H_OUT - H0_T0_OUT) *
                     (H1_rH - H0_rH) /
                     (H1_T0_OUT - H0_T0_OUT) +
                     H0_rH;

    // Clamp
    if (humidity < 0.0f) humidity = 0.0f;
    if (humidity > 100.0f) humidity = 100.0f;

    return humidity;

	/*int16_t H0_out, H1_out, H_out;
	    int16_t H0_rH, H1_rH;
	    uint8_t buffer[4];
	    float humidity = 0.0f;

{

	        // 1. Read calibration data (H0_rH_x2 and H1_rH_x2)
	    	if(HAL_I2C_Mem_Read(&hi2c2, HTS221_ADDR, 0x30 | 0x80, I2C_MEMADD_SIZE_8BIT, buffer, 2, I2C_TIMEOUT_MS)!=HAL_OK)
	        {
	        	                return NAN;   // ❌ I2C failed → do not block task
	        	        }
	        H0_rH = buffer[0] >> 1; // H0_rH is 8-bit, 0x30
	        H1_rH = buffer[1] >> 1; // H1_rH is 8-bit, 0x31

	        // 2. Read calibration data (H0_out and H1_out)
	        if(HAL_I2C_Mem_Read(&hi2c2, HTS221_ADDR, 0x36 | 0x80, I2C_MEMADD_SIZE_8BIT, buffer, 4, I2C_TIMEOUT_MS)!=HAL_OK)
	        {
	        	                return NAN;   // ❌ I2C failed → do not block task
	        	        }
	        H0_out = (((int16_t)buffer[1]) << 8) | (int16_t)buffer[0];
	        H1_out = (((int16_t)buffer[3]) << 8) | (int16_t)buffer[2];

	        // 3. Read current humidity raw data
	        if(HAL_I2C_Mem_Read(&hi2c2, HTS221_ADDR, 0x28 | 0x80, I2C_MEMADD_SIZE_8BIT, buffer, 2, I2C_TIMEOUT_MS)!=HAL_OK)
	        {
	        	                return NAN;   // ❌ I2C failed → do not block task
	        	        }
	        H_out = (((int16_t)buffer[1]) << 8) | (int16_t)buffer[0];

	        // 4. Linear interpolation
	        humidity = (float)(H_out - H0_out) * (float)(H1_rH - H0_rH) / (float)(H1_out - H0_out) + (float)H0_rH;

	        // Clamp values to 0-100%
	        if (humidity > 100.0f) humidity = 100.0f;
	        if (humidity < 0.0f) humidity = 0.0f;


	    }
	    return humidity;*/
}


static float Read_LPS22HB_Pressure()
{
	uint8_t buffer[3];
	    int32_t raw;

	    if (HAL_I2C_Mem_Read(&hi2c2,
	                         LPS22HB_ADDR,
	                         0x28 | 0x80,
	                         I2C_MEMADD_SIZE_8BIT,
	                         buffer, 3, 10) != HAL_OK)
	    {
	        return NAN;
	    }
	    raw = (int32_t)((buffer[2] << 16) | (buffer[1] << 8) | buffer[0]);
	    raw = (raw << 8) >> 8;   // sign extend

	    return raw / 4096.0f;

	/*// LPS22HB pressure is a 24-bit value across 3 registers (OUT_P_XL, L, H)
	    int32_t raw_pressure = 0;
	    float pressure_hPa = 0.0f;
	    uint8_t buffer[3];


	        // Read 3 bytes starting from OUT_P_XL (0x28) with auto-increment (0x80)
	        // We read LSB, mid-byte, MSB
	    if(HAL_I2C_Mem_Read(&hi2c2, LPS22HB_ADDR, 0x28 | 0x80, I2C_MEMADD_SIZE_8BIT, buffer, 3, I2C_TIMEOUT_MS)!=HAL_OK)
        {
                return NAN;   // ❌ I2C failed → do not block task
        }

	        // Reconstruct the 24-bit raw data (stored as 2's complement)
	        raw_pressure = (int32_t)((uint32_t)buffer[2] << 16) | ((uint32_t)buffer[1] << 8) | (uint32_t)buffer[0];

	        // Conversion formula: raw value divided by 4096 gives pressure in hPa
	        pressure_hPa = (float)raw_pressure / 4096.0f;


	    return pressure_hPa;*/
}

static uint16_t AQI_Linear(float C,
                           float C_lo, float C_hi,
                           uint16_t I_lo, uint16_t I_hi)
{
    return (uint16_t)(
        ((I_hi - I_lo) * (C - C_lo)) / (C_hi - C_lo) + I_lo
    );
}

static uint16_t AQI_PM25(float pm25)
{
    if (pm25 <= 12.0f)
        return AQI_Linear(pm25, 0, 12, 0, 50);
    else if (pm25 <= 35.4f)
        return AQI_Linear(pm25, 12.1f, 35.4f, 51, 100);
    else if (pm25 <= 55.4f)
        return AQI_Linear(pm25, 35.5f, 55.4f, 101, 150);
    else
        return AQI_Linear(pm25, 55.5f, 150.0f, 151, 200);
}

static uint16_t AQI_CO2(uint16_t co2)
{
    if (co2 <= 600)
        return AQI_Linear(co2, 0, 600, 0, 50);
    else if (co2 <= 1000)
        return AQI_Linear(co2, 601, 1000, 51, 100);
    else if (co2 <= 2000)
        return AQI_Linear(co2, 1001, 2000, 101, 150);
    else
        return 200;
}

static uint16_t AQI_TVOC(uint16_t tvoc)
{
    if (tvoc <= 200)
        return AQI_Linear(tvoc, 0, 200, 0, 50);
    else if (tvoc <= 500)
        return AQI_Linear(tvoc, 201, 500, 51, 100);
    else if (tvoc <= 1000)
        return AQI_Linear(tvoc, 501, 1000, 101, 150);
    else
        return 200;
}






static void I2C_Scan(I2C_HandleTypeDef *hi2c)
	    		        {
	    		            char msg[32];

	    		            for (uint8_t addr = 1; addr < 127; addr++)
	    		            {
	    		                if (HAL_I2C_IsDeviceReady(hi2c, addr << 1, 1, 10) == HAL_OK)
	    		                {
	    		                    sprintf(msg, "I2C found: 0x%02X\r\n", addr);
	    		                    HAL_UART_Transmit(&huart1,
	    		                                      (uint8_t *)msg,
	    		                                      strlen(msg),
	    		                                      100);
	    		                }
	    		            }
	    		        }

static void SensorReadTask(void * parameter)
{
	    // One-time init
	    if (xSemaphoreTake(sensorDataMutex, portMAX_DELAY) == pdTRUE)
	    {
	    	//uint8_t product_type = 0;
	    	//uint8_t product_version = 0;
	    	//uint8_t rx;



	        Init_HTS221();
	        Init_LPS22HB();

	        // ✅ Call the scan here
	        I2C_Scan(&hi2c1);


	        pms.PMS_huart = &huart4;          // UART4 is correct
	        pms.PMS_MODE  = PMS_MODE_ACTIVE;

	        if (PMS_Init(&pms) == PMS_OK)
	        {
	            HAL_UART_Transmit(&huart1,
	                (uint8_t*)"PMS initialization success\r\n", 13, 100);
	        }
	        else
	        {
	            HAL_UART_Transmit(&huart1,
	                (uint8_t*)"PMS initialization FAILED\r\n", 17, 100);
	        }



	        /* ---------- METHOD 3: Raw SGP30 I2C check ---------- */
	       /* uint8_t cmd[2] = { 0x20, 0x2F };   // GET_FEATURE_SET
	        uint8_t resp[6] = {0};
	        char msg[64];

	        if (HAL_I2C_Master_Transmit(&hi2c1, (0x58 << 1), cmd, 2, 100) == HAL_OK)
	        {
	            vTaskDelay(pdMS_TO_TICKS(10));

	            if (HAL_I2C_Master_Receive(&hi2c1, (0x58 << 1), resp, 6, 100) == HAL_OK)
	            {
	                sprintf(msg,
	                        "SGP30 raw: %02X %02X %02X %02X %02X %02X\r\n",
	                        resp[0], resp[1], resp[2],
	                        resp[3], resp[4], resp[5]);
	            }
	            else
	            {
	                sprintf(msg, "SGP30 RX failed\r\n");
	            }
	        }
	        else
	        {
	            sprintf(msg, "SGP30 TX failed\r\n");
	        }

	        HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 100);*/
	        /* ---------- END METHOD 3 ---------- */

	        if (sgp30_init(&sgp30_handle) != 0)
	        {
	            // SGP30 init failed
	        }

	        if (sgp30_iaq_init(&sgp30_handle) != 0)
	        {
	            // IAQ init failed
	        }

	        /*if (sgp30_get_feature_set(&sgp30_handle,
	                                  &product_type,
	                                  &product_version) == 0)
	        {
	            char msg[60];
	            sprintf(msg,
	                    "SGP30 type=0x%02X version=0x%02X\r\n",
	                    product_type,
	                    product_version);
	            HAL_UART_Transmit(&huart1,
	                              (uint8_t*)msg,
	                              strlen(msg),
	                              100);
	        }
	        else
	        {
	            HAL_UART_Transmit(&huart1,
	                (uint8_t*)"SGP30 feature read FAILED\r\n",
	                26,
	                100);
	        }*/
	        xSemaphoreGive(sensorDataMutex);
	    }

	    for (;;)
	    {
	        float t = NAN, h = NAN, p = NAN;
	        uint16_t tvoc = 0, co2 = 0;


	        /*if (sgp30_measure_iaq(&sgp30_handle, &co2, &tvoc) == 0)
	        {
	            if (xSemaphoreTake(sensorDataMutex, pdMS_TO_TICKS(10)) == pdTRUE)
	            {
	                currentCO2eq = co2;
	                currentTVOC  = tvoc;
	                xSemaphoreGive(sensorDataMutex);
	            }
	        }*/
	        /* --- 1. Read SGP30 (NO mutex) --- */
	        if (sgp30_measure_iaq(&sgp30_handle, &co2, &tvoc) == 0)
	        {
	        	currentCO2eq = co2;
				currentTVOC  = tvoc;
			}

	            /* --- 2. Update warm-up timer --- */
	            if (sgp30_seconds < 30)
	                sgp30_seconds++;

	            if (sgp30_seconds >= 15)
	                sgp30_ready = 1;

	            /*while (xQueueReceive(pmsRxQueue, &rx, 0) == pdPASS)
	            	            {
	            	                PMS5003_ParseByte(rx);   // or your parse function
	            	            }*/


	        // --- Read Other sensors (mutex protected)WITHOUT blocking ---
	        if (xSemaphoreTake(sensorDataMutex, pdMS_TO_TICKS(50)) == pdTRUE)
	        {
	            t = Read_HTS221_Temperature();
	            h = Read_HTS221_Humidity();
	            p = Read_LPS22HB_Pressure();

	            // ✅ Update globals ONLY if valid
	            if (!isnan(t)) currentTemperature = t;
	            if (!isnan(h)) currentHumidity    = h;
	            if (!isnan(p)) currentPressure    = p;


	            static uint8_t pms_counter = 0;

	            /* PMS every 5 seconds */
	            pms_counter++;
	            if (pms_counter >= 5)   // every 5 seconds
	            {
	                pms_counter = 0;
	                if (PMS_read(&pms) == PMS_OK)
	                {
	                    currentPM2_5 = pms.PM2_5_atmospheric;
	                    currentPM10  = pms.PM10_atmospheric;
	                }
	            }
	            /* ---- Air Quality Evaluation using PM2.5 LED2 ---- */

	            /*if (currentPM2_5 <= PM25_GOOD_MAX)
	            {
	                // GOOD air → LED OFF
	                HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
	            }
	            else if (currentPM2_5 <= PM25_MODERATE_MAX)
	            {
	                // MODERATE air → LED ON (steady)
	                HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
	            }
	            else
	            {
	                // POOR air → LED BLINK
	                HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
	            }*/
	            /* ---- Air Quality Evaluation using AQI Index LED2 ---- */

	            uint16_t aqi = Calculate_AQI();

	            if (aqi <= 50)
	            {
	                // GOOD → LED OFF (green implied)
	                HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
	            }
	            else if (aqi <= 100)
	            {
	                // MODERATE → slow blink (yellow)
	                HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
	                vTaskDelay(pdMS_TO_TICKS(500));
	            }
	            else
	            {
	                // UNHEALTHY → fast blink (red)
	                HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
	                vTaskDelay(pdMS_TO_TICKS(50));

	            }




	            /*if (!isnan(t)) currentTemperature = t;
	            if (!isnan(h)) currentHumidity    = 55.5;
	            if (!isnan(p)) currentPressure    = 1012.3;
	            currentTVOC  = 120;
	            currentCO2eq = 450;*/

	            xSemaphoreGive(sensorDataMutex);
	        }

	        vTaskDelay(pdMS_TO_TICKS(1000));
	    }
	}


/*

static void HumidPressRead(void * parameter)
{
    Init_LPS22HB();
    while(1)
    {
        currentHumidity = Read_HTS221_Humidity();
        currentPressure = Read_LPS22HB_Pressure();
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

*/

static void SerialMonitorTask(void *parameter)
{
    char buffer[128];
    // Local copies of variables for consistent data
    float temp, hum, pres;
    uint16_t tvoc, co2, pm25, pm10;

    for (;;)
    {
        // --- Acquire Mutex before reading global data ---
        //if (xSemaphoreTake(sensorDataMutex, portMAX_DELAY) == pdTRUE)
    	if (xSemaphoreTake(sensorDataMutex, pdMS_TO_TICKS(50)) == pdTRUE)
        {
            temp = currentTemperature;
            hum = currentHumidity;
            pres = currentPressure;
            tvoc = currentTVOC;
            co2 = currentCO2eq;
            pm25 = currentPM2_5;
            pm10 = currentPM10;

            xSemaphoreGive(sensorDataMutex); // Release Mutex immediately
        }
        // --- End Mutex section ---

        // Format consistent sensor values into a string using local variables
        sprintf(buffer,
                "Temp: %.1f C | Hum: %.1f %% | Pres: %.1f hPa\r\n"
                "TVOC: %u ppb | CO2: %u ppm\r\n"
                "PM2.5: %u | PM10: %u\r\n",
                temp,
                hum,
                pres,
                tvoc,
                co2,
                pm25,
                pm10);

        // Send over UART1 (connected to your PC serial monitor)
        HAL_UART_Transmit(&huart1, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}



static void LCDDisplay(void * parameter)
{
    char buffer[64];
    float temp, hum, pres;
    uint16_t tvoc, co2, pm25, pm10;

    ST7735_FillScreen(ST7735_BLACK);
    ST7735_DrawString(0, 0, "AirQualityMonitor",
                      Font_7x10, ST7735_WHITE, ST7735_BLACK);

    for (;;)
    {
        if (xSemaphoreTake(sensorDataMutex, pdMS_TO_TICKS(50)) == pdTRUE)
        {
            temp = currentTemperature;
            hum  = currentHumidity;
            pres = currentPressure;
            tvoc = currentTVOC;
            co2  = currentCO2eq;
            pm25 = currentPM2_5;
            pm10 = currentPM10;
            xSemaphoreGive(sensorDataMutex);
        }

        sprintf(buffer, "Temp: %.1f C   ", temp);
        ST7735_DrawString(0, 12, buffer, Font_7x10, ST7735_WHITE, ST7735_BLACK);

        sprintf(buffer, "Hum : %.1f %%  ", hum);
        ST7735_DrawString(0, 24, buffer, Font_7x10, ST7735_WHITE, ST7735_BLACK);

        sprintf(buffer, "Pres: %.1f hPa   ", pres);
        ST7735_DrawString(0, 36, buffer, Font_7x10, ST7735_WHITE, ST7735_BLACK);

        if (!sgp30_ready)
        {
            ST7735_DrawString(0, 60,
                "SGP30 warming up   ",
                Font_7x10,
                ST7735_WHITE,
                ST7735_BLACK);

            ST7735_DrawString(0, 72,
                "Please wait...     ",
                Font_7x10,
                ST7735_WHITE,
                ST7735_BLACK);
        }
        else
        {
            sprintf(buffer, "TVOC: %u ppb   ", tvoc);
            ST7735_DrawString(0, 60, buffer,
                Font_7x10, ST7735_WHITE, ST7735_BLACK);

            sprintf(buffer, "CO2 : %u ppm   ", co2);
            ST7735_DrawString(0, 72, buffer,
                Font_7x10, ST7735_WHITE, ST7735_BLACK);
        }

        // Optional PMS display (enable once PMS is stable)

         snprintf(buffer, sizeof(buffer), "PM2.5: %u ug/m3 ", pm25);
         ST7735_DrawString(0, 84, buffer,Font_7x10, ST7735_WHITE, ST7735_BLACK);

         char buf[32];
         uint16_t aqi = Calculate_AQI();

         sprintf(buf, "AQI: %u", aqi);
         ST7735_DrawString(0, 96, buf, Font_7x10, ST7735_WHITE, ST7735_BLACK);

         if (aqi <= 50)
             ST7735_DrawString(0, 108, "GOOD", Font_7x10, ST7735_GREEN, ST7735_BLACK);
         else if (aqi <= 100)
             ST7735_DrawString(0, 108, "MODERATE", Font_7x10, ST7735_YELLOW, ST7735_BLACK);
         else
             ST7735_DrawString(0, 108, "UNHEALTHY", Font_7x10, ST7735_RED, ST7735_BLACK);



        vTaskDelay(pdMS_TO_TICKS(2000));
    }

	 /*char buf[32];
	    int counter = 0;

	    ST7735_FillScreen(ST7735_BLACK);

	    ST7735_DrawString(0,  0, "AirQualityMonitor",
	                      Font_7x10, ST7735_WHITE, ST7735_BLACK);

	    for (;;)
	    {
	        sprintf(buf, "Counter: %d   ", counter++);
	        ST7735_DrawString(0, 20, buf,
	                          Font_7x10, ST7735_GREEN, ST7735_BLACK);

	        sprintf(buf, "FreeRTOS OK");
	        ST7735_DrawString(0, 40, buf,
	                          Font_7x10, ST7735_CYAN, ST7735_BLACK);

	        vTaskDelay(pdMS_TO_TICKS(1000));
	    }*/
}



	/*void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
	{
	    if (huart->Instance == UART4)
	    {
	        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	        // Push received byte into queue
	        if (pmsRxQueue != NULL)
	        {
	            xQueueSendFromISR(
	                pmsRxQueue,
	                &pms5003_rx_byte,
	                &xHigherPriorityTaskWoken
	            );
	        }

	        // Restart UART RX immediately
	        HAL_UART_Receive_IT(huart, &pms5003_rx_byte, 1);

	        // Context switch if needed
	        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	    }
	}*/




// You would modify SensorReadTask to receive this notification,
// process the PM data when notified, and update globals with mutex protection.


/*
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	//HAL_UART_Transmit(&huart1, (uint8_t *) msg1, strlen(msg1), 1000);
	//readComplete = 1;
	xTaskResumeFromISR(pTempSensorRead);
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	//HAL_UART_Transmit(&huart1, (uint8_t *) msg2, strlen(msg2), 1000);
	//writeComplete = 1;
	xTaskResumeFromISR(pTempSensorRead);
}
*/

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
