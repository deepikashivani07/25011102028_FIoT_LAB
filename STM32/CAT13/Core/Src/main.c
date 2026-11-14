/* USER CODE BEGIN Header */
/**
  * ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body (BME280 + ESP8266 HTTP Server for CAT 13)
  * ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "bme280.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart1; // Connected to ESP8266 Modem
UART_HandleTypeDef huart2; // Connected to PC Debug Terminal

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);

/* Helper to print debug messages to PC terminal */
static void UART_Print(char *text)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)text, strlen(text), HAL_MAX_DELAY);
}

/* BME280 I2C read callback */
static int8_t user_i2c_read(uint8_t reg, uint8_t *data, uint32_t len, void *intf_ptr)
{
    uint8_t dev_addr = *(uint8_t*)intf_ptr;
    if (HAL_I2C_Mem_Read(&hi2c1, (dev_addr << 1), reg, I2C_MEMADD_SIZE_8BIT, data, len, HAL_MAX_DELAY) == HAL_OK)
    {
        return BME280_OK;
    }
    return BME280_E_COMM_FAIL;
}

/* BME280 I2C write callback */
static int8_t user_i2c_write(uint8_t reg, const uint8_t *data, uint32_t len, void *intf_ptr)
{
    uint8_t dev_addr = *(uint8_t*)intf_ptr;
    if (HAL_I2C_Mem_Write(&hi2c1, (dev_addr << 1), reg, I2C_MEMADD_SIZE_8BIT, (uint8_t *)data, len, HAL_MAX_DELAY) == HAL_OK)
    {
        return BME280_OK;
    }
    return BME280_E_COMM_FAIL;
}

/* Bosch driver delay function in microseconds */
static void user_delay_us(uint32_t period, void *intf_ptr)
{
    HAL_Delay((period + 999U) / 1000U);
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_USART1_UART_Init();
    MX_USART2_UART_Init();

    /* USER CODE BEGIN 2 */
    struct bme280_dev dev;
    struct bme280_settings settings;
    struct bme280_data comp_data;

    static uint8_t dev_addr = BME280_I2C_ADDR_PRIM; // 0x76[cite: 3]
    int8_t rslt;

    char debug_msg[160];
    char cmd[100];
    char json_payload[200];
    char http_response[400];
    uint8_t rxBuf[256];
    uint32_t rxIndex = 0;

    UART_Print("\r\n--- STM32 BME280 IoT HTTP Server Starting --- \r\n");

    /* Configure BME280 Device Structure */
    dev.intf = BME280_I2C_INTF;
    dev.read = user_i2c_read;
    dev.write = user_i2c_write;
    dev.delay_us = user_delay_us;
    dev.intf_ptr = &dev_addr;

    /* Initialize BME280 Sensor */
    rslt = bme280_init(&dev);
    if (rslt != BME280_OK)
    {
        UART_Print("BME280 Initialization Failed!\r\n");
        while (1) { HAL_Delay(1000); }
    }
    UART_Print("BME280 Detected Successfully!\r\n");

    /* Configure Sensor Settings */
    settings.osr_h = BME280_OVERSAMPLING_1X;
    settings.osr_p = BME280_OVERSAMPLING_1X;
    settings.osr_t = BME280_OVERSAMPLING_1X;
    settings.filter = BME280_FILTER_COEFF_2;
    settings.standby_time = BME280_STANDBY_TIME_1000_MS;

    uint8_t settings_sel = BME280_SEL_OSR_HUM | BME280_SEL_OSR_PRESS | 
                           BME280_SEL_OSR_TEMP | BME280_SEL_FILTER | BME280_SEL_STANDBY;

    bme280_set_sensor_settings(settings_sel, &settings, &dev);
    bme280_set_sensor_mode(BME280_POWERMODE_NORMAL, &dev);

    /* Initialize ESP8266 Wi-Fi Module as TCP Server */
    UART_Print("Initializing ESP8266 Server...\r\n");
    HAL_Delay(1000);

    // Enable Multiple Connections
    char *muxCmd = "AT+CIPMUX=1\r\n";
    HAL_UART_Transmit(&huart1, (uint8_t*)muxCmd, strlen(muxCmd), HAL_MAX_DELAY);
    HAL_Delay(1000);

    // Start TCP Server on Port 80
    char *serverCmd = "AT+CIPSERVER=1,80\r\n";
    HAL_UART_Transmit(&huart1, (uint8_t*)serverCmd, strlen(serverCmd), HAL_MAX_DELAY);
    HAL_Delay(2000);

    UART_Print("[INFO] HTTP Server active on Port 80. Ready for client requests...\r\n");
    /* USER CODE END 2 */

    /* Infinite loop */
    while (1)
    {
        // 1. Read latest sensor data periodically
        rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, &dev);
        
        float temperature = comp_data.temperature; // °C
        float pressure = comp_data.pressure / 100.0f; // hPa
        float humidity = comp_data.humidity; // %RH

        if (rslt == BME280_OK)
        {
            snprintf(debug_msg, sizeof(debug_msg), 
                     "Temp: %.2f C | Hum: %.2f %%RH | Press: %.2f hPa\r\n", 
                     temperature, humidity, pressure);
            UART_Print(debug_msg);
        }

        // 2. Check for incoming client requests (+IPD) from ESP8266 over UART1
        uint8_t ch;
        if (HAL_UART_Receive(&huart1, &ch, 1, 100) == HAL_OK)
        {
            // Forward raw data to PC debug terminal
            HAL_UART_Transmit(&huart2, &ch, 1, HAL_MAX_DELAY);

            if (rxIndex < sizeof(rxBuf) - 1)
            {
                rxBuf[rxIndex++] = ch;
                rxBuf[rxIndex] = '\0';
            }

            // Detect HTTP Request Frame (+IPD)
            if (strstr((char*)rxBuf, "+IPD"))
            {
                int linkId = 0;
                char *ipdPtr = strstr((char*)rxBuf, "+IPD,");
                if (ipdPtr != NULL)
                {
                    linkId = ipdPtr[5] - '0'; // Extract connection link ID
                }

                HAL_Delay(50); // Allow frame completion

                // Construct Dynamic JSON Payload with BME280 Readings[cite: 8, 9]
                int json_len = snprintf(json_payload, sizeof(json_payload),
                    "{\"status\":\"success\",\"temperature\":%.2f,\"humidity\":%.2f,\"pressure\":%.2f}",
                    temperature, humidity, pressure);

                // Construct HTTP Response Header & Body[cite: 8, 9]
                int total_len = snprintf(http_response, sizeof(http_response),
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: application/json\r\n"
                    "Content-Length: %d\r\n"
                    "\r\n"
                    "%s", json_len, json_payload);

                // Step 1: Send CIPSEND command with link ID and total payload length[cite: 3, 9]
                snprintf(cmd, sizeof(cmd), "AT+CIPSEND=%d,%d\r\n", linkId, total_len);
                HAL_UART_Transmit(&huart1, (uint8_t*)cmd, strlen(cmd), HAL_MAX_DELAY);
                HAL_Delay(500);

                // Step 2: Transmit the HTTP JSON payload response[cite: 3]
                HAL_UART_Transmit(&huart1, (uint8_t*)http_response, total_len, HAL_MAX_DELAY);
                HAL_Delay(1000);

                // Close TCP Connection smoothly
                snprintf(cmd, sizeof(cmd), "AT+CIPCLOSE=%d\r\n", linkId);
                HAL_UART_Transmit(&huart1, (uint8_t*)cmd, strlen(cmd), HAL_MAX_DELAY);
                HAL_Delay(500);

                // Reset request buffer
                rxIndex = 0;
                memset(rxBuf, 0, sizeof(rxBuf));
            }

            if (rxIndex >= sizeof(rxBuf) - 1)
            {
                rxIndex = 0;
                memset(rxBuf, 0, sizeof(rxBuf));
            }
        }

        HAL_Delay(1000); // Polling delay
    }
}

/**
  * @brief System Clock Configuration
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 84;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    RCC_OscInitStruct.PLL.PLLR = 2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

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

/**
  * @brief I2C1 Initialization Function
  */
static void MX_I2C1_Init(void)
{
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
}

/**
  * @brief USART1 Initialization Function (ESP8266 Wi-Fi)
  */
static void MX_USART1_UART_Init(void)
{
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief USART2 Initialization Function (PC Debug Terminal)
  */
static void MX_USART2_UART_Init(void)
{
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief GPIO Initialization Function
  */
static void MX_GPIO_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
}

/**
  * @brief  This function is executed in case of error occurrence.
  */
void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}