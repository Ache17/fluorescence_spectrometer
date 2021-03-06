/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_host.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define false 0
#define true  1
typedef int bool;

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c3;

I2S_HandleTypeDef hi2s2;
I2S_HandleTypeDef hi2s3;

SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */
int st = 0;
#define SPECTR_ADDR 0x39 << 1

#define WHOAMI_ADRR     0x92
#define ENABLE_ADDR     0x80
#define CONFIG_ADDR     0x70
#define ATIME_ADDR      0x81
#define ASTEP_ADDR_LSB  0xCA
#define ASTEP_ADDR_MSB  0xCB
#define CFG0_ADDR       0xA9
#define LED_ADDR        0x74
#define AS7341_CFG6     0xAF
#define AS7341_STATUS2  0xA3
#define AS7341_CH0_DATA_L     0x95 

#define CFG0_VAL_REG0         0b00000000  //  REGISTER BANK >= 0x80
#define CFG0_VAL_REG1         0b00010000  //  REGISTER BANK 0x60 - 0x74   
#define TURN_ON_LED           0b00001000  // TURN LED ON
#define TURN_OFF_LED          0b00000000  // TURN LED ON
#define LED_VAL               0b10000100  // LED POWER CONTROL
#define ENABLE_VAL            0b00000011  // POWER, SPM ENABLED | SMUX WAIT, FLICKER DISABLED
#define SMUX_ENABLE           0b00010011  // POWER, SPM, SMUX ENABLED |  WAIT, FLICKER DISABLED
#define SPM_ON                0b00000011
#define SPM_OFF               0b00000001
#define AS7341_SMUX_CMD_WRITE 0b00000010


uint8_t d1[1] = {"\x92"};
uint8_t buffor[1];
uint8_t reading[24];
HAL_StatusTypeDef res;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2S2_Init(void);
static void MX_I2S3_Init(void);
static void MX_SPI1_Init(void);
static void MX_I2C3_Init(void);
void MX_USB_HOST_Process(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

HAL_StatusTypeDef writeRegisterByte(I2C_HandleTypeDef *hi2c, uint8_t address, uint8_t value)
{
  buffor[0] = value;
  res = HAL_I2C_Mem_Write_IT(hi2c, SPECTR_ADDR, address, 1,buffor,  1);
  if (res != HAL_OK)
    HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_SET);
  return res;
}
void setup_F1F4_Clear_NIR(I2C_HandleTypeDef *hi2c)
{
  writeRegisterByte(hi2c, 0x00, 0x30);  // F3 left set to ADC2
  writeRegisterByte(hi2c, 0x01, 0x01);  // F1 left set to ADC0
  writeRegisterByte(hi2c, 0x02, 0x00);  // Reserved or disabled
  writeRegisterByte(hi2c, 0x03, 0x00);  // F8 left disabled
  writeRegisterByte(hi2c, 0x04, 0x00);  // F6 left disabled
  writeRegisterByte(hi2c, 0x05, 0x42);  // F4 left connected to ADC3/f2 left connected to ADC1
  writeRegisterByte(hi2c, 0x06, 0x00);  // F5 left disabled
  writeRegisterByte(hi2c, 0x07, 0x00);  // F7 left disabled
  writeRegisterByte(hi2c, 0x08, 0x50);  // CLEAR connected to ADC4
  writeRegisterByte(hi2c, 0x09, 0x00);  // F5 right disabled
  writeRegisterByte(hi2c, 0x0A, 0x00);  // F7 right disabled
  writeRegisterByte(hi2c, 0x0B, 0x00);  // Reserved or disabled
  writeRegisterByte(hi2c, 0x0C, 0x20);  // F2 right connected to ADC1
  writeRegisterByte(hi2c, 0x0D, 0x04);  // F4 right connected to ADC3
  writeRegisterByte(hi2c, 0x0E, 0x00);  // F6/F8 right disabled
  writeRegisterByte(hi2c, 0x0F, 0x30);  // F3 right connected to ADC2
  writeRegisterByte(hi2c, 0x10, 0x01);  // F1 right connected to ADC0
  writeRegisterByte(hi2c, 0x11, 0x50);  // CLEAR right connected to ADC4
  writeRegisterByte(hi2c, 0x12, 0x00);  // Reserved or disabled
  writeRegisterByte(hi2c, 0x13, 0x06);  // NIR connected to ADC5
}
void setup_F5F8_Clear_NIR(I2C_HandleTypeDef *hi2c)
{
  writeRegisterByte(hi2c, 0x00, 0x00); // F3 left disable
  writeRegisterByte(hi2c, 0x01, 0x00); // F1 left disable
  writeRegisterByte(hi2c, 0x02, 0x00); // reserved/disable
  writeRegisterByte(hi2c, 0x03, 0x40); // F8 left connected to ADC3
  writeRegisterByte(hi2c, 0x04, 0x02); // F6 left connected to ADC1
  writeRegisterByte(hi2c, 0x05, 0x00); // F4/ F2 disabled
  writeRegisterByte(hi2c, 0x06, 0x10); // F5 left connected to ADC0
  writeRegisterByte(hi2c, 0x07, 0x03); // F7 left connected to ADC2
  writeRegisterByte(hi2c, 0x08, 0x50); // CLEAR Connected to ADC4
  writeRegisterByte(hi2c, 0x09, 0x10); // F5 right connected to ADC0
  writeRegisterByte(hi2c, 0x0A, 0x03); // F7 right connected to ADC2
  writeRegisterByte(hi2c, 0x0B, 0x00); // Reserved or disabled
  writeRegisterByte(hi2c, 0x0C, 0x00); // F2 right disabled
  writeRegisterByte(hi2c, 0x0D, 0x00); // F4 right disabled
  writeRegisterByte(hi2c, 0x0E, 0x24); // F8 right connected to ADC2/ F6 right connected to ADC1
  writeRegisterByte(hi2c, 0x0F, 0x00); // F3 right disabled
  writeRegisterByte(hi2c, 0x10, 0x00); // F1 right disabled
  writeRegisterByte(hi2c, 0x11, 0x50); // CLEAR right connected to AD4
  writeRegisterByte(hi2c, 0x12, 0x00); // Reserved or disabled
  writeRegisterByte(hi2c, 0x13, 0x06); // NIR connected to ADC5
}
HAL_StatusTypeDef getID(I2C_HandleTypeDef *hi2c, uint8_t *data)
{
  return HAL_I2C_Mem_Read(&hi2c1, SPECTR_ADDR, WHOAMI_ADRR, 1, d1, 1, HAL_MAX_DELAY);
}
HAL_StatusTypeDef setupIntegration(I2C_HandleTypeDef *hi2c, uint8_t ATIME, uint16_t ASTEP)
{
  uint16_t ASTEP_LSB = ASTEP & 0xff; 
  uint16_t ASTEP_MSB = (ASTEP >> 8);

  buffor[0] = ATIME;
  res = HAL_I2C_Mem_Write(hi2c, SPECTR_ADDR, ATIME_ADDR, 1, buffor, 1, HAL_MAX_DELAY);
  if (res != HAL_OK)
    return res;
  
  buffor[0] = (uint8_t)ASTEP_LSB;
  res = HAL_I2C_Mem_Write(hi2c, SPECTR_ADDR, ASTEP_ADDR_LSB, 1, buffor, 1, HAL_MAX_DELAY);
  if (res != HAL_OK)
    return res;

  buffor[0] = (uint8_t)ASTEP_MSB;
  res = HAL_I2C_Mem_Write(hi2c, SPECTR_ADDR, ASTEP_ADDR_MSB, 1, buffor, 1, HAL_MAX_DELAY);
  if (res != HAL_OK)
    return res;

  return HAL_OK;
}
HAL_StatusTypeDef chipEnable(I2C_HandleTypeDef *hi2c)
{
  // SMUX, POWER, SPM ENABLED | WAIT, FLICKER DISABLED
  buffor[0] = (uint8_t)ENABLE_VAL;
  return HAL_I2C_Mem_Write(hi2c, SPECTR_ADDR, ENABLE_ADDR, 1, buffor, 1, HAL_MAX_DELAY);
}
HAL_StatusTypeDef turnOnLED(I2C_HandleTypeDef *hi2c)
{
  buffor[0] = CFG0_VAL_REG1;
  res = HAL_I2C_Mem_Write(hi2c, SPECTR_ADDR, CFG0_ADDR, 1, buffor, 1, HAL_MAX_DELAY);
  if (res != HAL_OK)
    return res;

  buffor[0] = TURN_ON_LED;
  res = HAL_I2C_Mem_Write(hi2c, SPECTR_ADDR, CONFIG_ADDR, 1, buffor, 1, HAL_MAX_DELAY);
  if (res != HAL_OK)
    return res;

  buffor[0] = LED_VAL;
  res = HAL_I2C_Mem_Write(hi2c, SPECTR_ADDR, LED_ADDR, 1, buffor, 1, HAL_MAX_DELAY);
  if (res != HAL_OK)
    return res;

  return HAL_OK;
}
HAL_StatusTypeDef turnOffLED(I2C_HandleTypeDef *hi2c)
{
  buffor[0] = CFG0_VAL_REG1;
  res = HAL_I2C_Mem_Write(hi2c, SPECTR_ADDR, CFG0_ADDR, 1, buffor, 1, HAL_MAX_DELAY);
  if (res != HAL_OK)
    return res;

  buffor[0] = TURN_OFF_LED;
  res = HAL_I2C_Mem_Write(hi2c, SPECTR_ADDR, CONFIG_ADDR, 1, buffor, 1, HAL_MAX_DELAY);
  if (res != HAL_OK)
    return res;

  buffor[0] = 0;
  res = HAL_I2C_Mem_Write(hi2c, SPECTR_ADDR, LED_ADDR, 1, buffor, 1, HAL_MAX_DELAY);
  if (res != HAL_OK)
    return res;

  return HAL_OK;
}
HAL_StatusTypeDef enableSpectralMeasurement(I2C_HandleTypeDef *hi2c, bool enable)
{
  if (enable)
    buffor[0] = SPM_ON;
  else
    buffor[0] = SPM_OFF;
  return HAL_I2C_Mem_Write(hi2c, SPECTR_ADDR, ENABLE_ADDR, 1, buffor, 1, HAL_MAX_DELAY);
}
void setSMUXCommand(I2C_HandleTypeDef *hi2c, uint8_t command)
{
  writeRegisterByte(hi2c, AS7341_CFG6, command);
}
HAL_StatusTypeDef enableSMUX(I2C_HandleTypeDef *hi2c)
{
    return writeRegisterByte(hi2c, ENABLE_ADDR, SMUX_ENABLE);
}
void setSMUXLowChannels(I2C_HandleTypeDef *hi2c, bool f1_f4)
{ 
  enableSpectralMeasurement(hi2c, false);
  setSMUXLowChannels(hi2c, AS7341_SMUX_CMD_WRITE);
  if (f1_f4)
  {
    setup_F1F4_Clear_NIR(hi2c);
  }
  else
  {
    setup_F5F8_Clear_NIR(hi2c);
  }

  if (enableSMUX(hi2c) != HAL_OK)
    HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_SET);
}
bool isDataReady(I2C_HandleTypeDef *hi2c)
{
  if (HAL_I2C_Mem_Read(hi2c, SPECTR_ADDR, AS7341_STATUS2, 1, buffor, 1, HAL_MAX_DELAY) != HAL_OK)
    HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_SET);

  return (buffor[0] >> 6) & 1;
}
void waitForData(I2C_HandleTypeDef *hi2c)
{
  HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_SET);
  while(! isDataReady(hi2c))
  {}
  HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_RESET);
}
void readAllChannel(I2C_HandleTypeDef *hi2c)
{
  setSMUXLowChannels(hi2c, true);
  enableSpectralMeasurement(hi2c, true);
  waitForData(hi2c);

  if (HAL_I2C_Mem_Read(hi2c, SPECTR_ADDR, AS7341_CH0_DATA_L, 1, reading, 12, HAL_MAX_DELAY) != HAL_OK)
  {
    HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_SET);
  }
}

/*
*
*   LD6 BLUE   LED
*   LD5 RED    LED
*   LD4 ORANGE LED
*   LD3 GREEN  LED
*
*/


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
  MX_I2S2_Init();
  MX_I2S3_Init();
  MX_SPI1_Init();
  MX_USB_HOST_Init();
  MX_I2C3_Init();

  /* USER CODE BEGIN 2 */
  /* USER CODE END 2 */
  HAL_Delay(1);
  if (getID(&hi2c1, d1) != HAL_OK)
      HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);

  if (getID(&hi2c1, d1) != HAL_OK)
      HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);

  if (chipEnable(&hi2c1) != HAL_OK)
    HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_SET);

  if (turnOnLED(&hi2c1) != HAL_OK)
    HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_SET);

  HAL_Delay(200);
  if (turnOffLED(&hi2c1) != HAL_OK)
    HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_SET);

//  if (HAL_I2C_Mem_Write(&hi2c1, SPECTR_ADDR, 0x80, 1, d2, 1, HAL_MAX_DELAY) != HAL_OK)
//  {
//      HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_SET);
//  }
  //  HAL_I2C_Master_Transmit(&hi2c1, spectroAddr, )

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET)
    {
      HAL_GPIO_WritePin(Toggle_f_GPIO_Port, Toggle_f_Pin, GPIO_PIN_RESET);
      readAllChannel(&hi2c1);
    }
    else
    {
      HAL_GPIO_WritePin(Toggle_f_GPIO_Port, Toggle_f_Pin, GPIO_PIN_SET);
    }
    

    /* USER CODE END WHILE */
    MX_USB_HOST_Process();

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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 8;
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
  PeriphClkInitStruct.PLLI2S.PLLI2SN = 200;
  PeriphClkInitStruct.PLLI2S.PLLI2SM = 5;
  PeriphClkInitStruct.PLLI2S.PLLI2SR = 2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
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
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.ClockSpeed = 100000;
  hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief I2S2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2S2_Init(void)
{

  /* USER CODE BEGIN I2S2_Init 0 */

  /* USER CODE END I2S2_Init 0 */

  /* USER CODE BEGIN I2S2_Init 1 */

  /* USER CODE END I2S2_Init 1 */
  hi2s2.Instance = SPI2;
  hi2s2.Init.Mode = I2S_MODE_MASTER_TX;
  hi2s2.Init.Standard = I2S_STANDARD_PHILIPS;
  hi2s2.Init.DataFormat = I2S_DATAFORMAT_16B;
  hi2s2.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
  hi2s2.Init.AudioFreq = I2S_AUDIOFREQ_96K;
  hi2s2.Init.CPOL = I2S_CPOL_LOW;
  hi2s2.Init.ClockSource = I2S_CLOCK_PLL;
  hi2s2.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_ENABLE;
  if (HAL_I2S_Init(&hi2s2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2S2_Init 2 */

  /* USER CODE END I2S2_Init 2 */

}

/**
  * @brief I2S3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2S3_Init(void)
{

  /* USER CODE BEGIN I2S3_Init 0 */

  /* USER CODE END I2S3_Init 0 */

  /* USER CODE BEGIN I2S3_Init 1 */

  /* USER CODE END I2S3_Init 1 */
  hi2s3.Instance = SPI3;
  hi2s3.Init.Mode = I2S_MODE_MASTER_TX;
  hi2s3.Init.Standard = I2S_STANDARD_PHILIPS;
  hi2s3.Init.DataFormat = I2S_DATAFORMAT_16B;
  hi2s3.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
  hi2s3.Init.AudioFreq = I2S_AUDIOFREQ_96K;
  hi2s3.Init.CPOL = I2S_CPOL_LOW;
  hi2s3.Init.ClockSource = I2S_CLOCK_PLL;
  hi2s3.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
  if (HAL_I2S_Init(&hi2s3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2S3_Init 2 */

  /* USER CODE END I2S3_Init 2 */

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
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_I2C_SPI_GPIO_Port, CS_I2C_SPI_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(OTG_FS_PowerSwitchOn_GPIO_Port, OTG_FS_PowerSwitchOn_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin
                          |Audio_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Toggle_f_GPIO_Port, Toggle_f_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : DATA_Ready_Pin */
  GPIO_InitStruct.Pin = DATA_Ready_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DATA_Ready_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : CS_I2C_SPI_Pin */
  GPIO_InitStruct.Pin = CS_I2C_SPI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CS_I2C_SPI_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : INT1_Pin INT2_Pin MEMS_INT2_Pin */
  GPIO_InitStruct.Pin = INT1_Pin|INT2_Pin|MEMS_INT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : OTG_FS_PowerSwitchOn_Pin Toggle_f_Pin */
  GPIO_InitStruct.Pin = OTG_FS_PowerSwitchOn_Pin|Toggle_f_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LD4_Pin LD3_Pin LD5_Pin LD6_Pin
                           Audio_RST_Pin */
  GPIO_InitStruct.Pin = LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin
                          |Audio_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : OTG_FS_OverCurrent_Pin */
  GPIO_InitStruct.Pin = OTG_FS_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(OTG_FS_OverCurrent_GPIO_Port, &GPIO_InitStruct);

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
