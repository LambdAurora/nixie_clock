/*
 * Copyright © 2020 LambdAurora <aurora42lambda@gmail.com>
 *
 * This file is part of lambda_nixie_clock.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include <main.h>

/**
 * Initializes the Global MSP.
 */
void HAL_MspInit(void) {
    // Activates clocks.
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
}

/**
 * Initializes the I2C.
 * @param hi2c I2C handle pointer.
 */
void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (hi2c->Instance == I2C1) {
        __HAL_RCC_GPIOB_CLK_ENABLE();

        /** I2C1 GPIO Configuration
         * PB6 ------> I2C1_SCL
         * PB7 ------> I2C1_SDA
         */
        GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF1_I2C1;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* Peripheral clock enable */
        __HAL_RCC_I2C1_CLK_ENABLE();
    } else if (hi2c->Instance == I2C2) {
        __HAL_RCC_GPIOB_CLK_ENABLE();

        /** I2C2 GPIO Configuration
         * PB10 ------> I2C2_SCL
         * PB11 ------> I2C2_SDA
         */
        GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF1_I2C2;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* Peripheral clock enable */
        __HAL_RCC_I2C2_CLK_ENABLE();
    }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c) {
    if (hi2c->Instance == I2C1) {
        /* Peripheral clock disable */
        __HAL_RCC_I2C1_CLK_DISABLE();

        /** I2C1 GPIO Configuration
         * PB6 ------> I2C1_SCL
         * PB7 ------> I2C1_SDA
         */
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6 | GPIO_PIN_7);
    } else if (hi2c->Instance == I2C2) {
        /* Peripheral clock disable */
        __HAL_RCC_I2C2_CLK_DISABLE();

        /** I2C2 GPIO Configuration
         * PB10 ------> I2C2_SCL
         * PB11 ------> I2C2_SDA
         */
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10 | GPIO_PIN_11);
    }
}

/**
 * Initializes the UART MSP.
 */
void HAL_UART_MspInit(UART_HandleTypeDef* huart) {
    GPIO_InitTypeDef gpio_init_struct;

    // Part1: enable clocks.
    /* Enable GPIO TX/RX clock */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* Enable USART 1 clock */
    __HAL_RCC_USART1_CLK_ENABLE();

    // Part 2: configure GPIO.

    /* UART TX GPIO pin configuration  */
    gpio_init_struct.Pin = USART1_TX_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init_struct.Alternate = USART1_TX_AF;

    HAL_GPIO_Init(USART1_TX_GPIO_PORT, &gpio_init_struct);

    /* UART RX GPIO pin configuration  */
    gpio_init_struct.Pin = USART1_RX_PIN;
    gpio_init_struct.Alternate = USART1_RX_AF;

    HAL_GPIO_Init(USART1_RX_GPIO_PORT, &gpio_init_struct);
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart) {
    // Part 1: Reset peripherals.
    __HAL_RCC_USART1_FORCE_RESET();
    __HAL_RCC_USART1_RELEASE_RESET();

    // Part 2: disable peripherals and clocks.
    HAL_GPIO_DeInit(USART1_TX_GPIO_PORT, USART1_TX_PIN);
    HAL_GPIO_DeInit(USART1_RX_GPIO_PORT, USART1_RX_PIN);
}
