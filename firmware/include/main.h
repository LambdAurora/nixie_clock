/*
 * Copyright © 2020 LambdAurora <aurora42lambda@gmail.com>
 *
 * This file is part of lambda_nixie_clock.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef LAMBDA_NIXIE_CLOCK_MAIN_H
#define LAMBDA_NIXIE_CLOCK_MAIN_H

#include <common.h>

#ifdef __cplusplus
#include <console.hpp>
#endif

#define EE24_ADDRESS (0x57 << 1)
/* Definition for USARTx Pins */
// TX -> PA9
// RX -> PA10
#define USART1_TX_PIN                    GPIO_PIN_9
#define USART1_TX_GPIO_PORT              GPIOA
#define USART1_TX_AF                     GPIO_AF1_USART1
#define USART1_RX_PIN                    GPIO_PIN_10
#define USART1_RX_GPIO_PORT              GPIOA
#define USART1_RX_AF                     GPIO_AF1_USART1

/* Definition for shift registers */
// PA1 -> SDI
#define SDI_PIN         GPIO_PIN_1
#define SDI_GPIO_PORT   GPIOA
// PA2 -> RCLK
#define RCLK_PIN        GPIO_PIN_2
#define RCLK_GPIO_PORT  GPIOA
// PA3 -> SRCLK
#define SRCLK_PIN       GPIO_PIN_3
#define SRCLK_GPIO_PORT GPIOA

#endif //LAMBDA_NIXIE_CLOCK_MAIN_H
