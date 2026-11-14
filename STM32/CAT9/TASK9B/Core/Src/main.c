/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body for Task 9B - UART Command Line Interface
  ******************************************************************************
  */
/* USER CODE END Header */
#include "main.h"
#include <string.h>
#include <stdio.h>

UART_HandleTypeDef huart1;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART1_UART_Init();

    char rxBuffer[20];
    char txMsg[50];
    uint8_t rxIndex = 0;
    uint8_t rxByte;

    sprintf(txMsg, "STM32 UART CLI Ready. Send: ON, OFF, TOGGLE\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)txMsg, strlen(txMsg), HAL_MAX_DELAY);

    while (1)
    {
        if (HAL_UART_Receive(&huart1, &rxByte, 1, 100) == HAL_OK)
        {
            HAL_UART_Transmit(&huart1, &rxByte, 1, HAL_MAX_DELAY);

            if (rxByte == '\r' || rxByte == '\n')
            {
                rxBuffer[rxIndex] = '\0'; 

                if (strlen(rxBuffer) > 0)
                {
                    if (strcasecmp(rxBuffer, "ON") == 0)
                    {
                        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET); 
                        sprintf(txMsg, "\r\n-> LED Turned ON\r\n");
                    }
                    else if (strcasecmp(rxBuffer, "OFF") == 0)
                    {
                        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);   
                        sprintf(txMsg, "\r\n-> LED Turned OFF\r\n");
                    }
                    else if (strcasecmp(rxBuffer, "TOGGLE") == 0)
                    {
                        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
                        sprintf(txMsg, "\r\n-> LED Toggled\r\n");
                    }
                    else
                    {
                        sprintf(txMsg, "\r\n-> Unknown Command: %s\r\n", rxBuffer);
                    }

                    HAL_UART_Transmit(&huart1, (uint8_t*)txMsg, strlen(txMsg), HAL_MAX_DELAY);
                }

                rxIndex = 0; 
                memset(rxBuffer, 0, sizeof(rxBuffer));
            }
            else if (rxIndex < sizeof(rxBuffer) - 1)
            {
                rxBuffer[rxIndex++] = rxByte;
            }
        }
    }
}

/**
  * @brief System Clock Configuration
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

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
  * @brief USART1 Initialization Function
  */
static void MX_USART1_UART_Init(void)
{
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 9600;
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
  * @brief GPIO Initialization Function
  */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}