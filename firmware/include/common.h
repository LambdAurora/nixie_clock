/*
 * Copyright © 2020 LambdAurora <aurora42lambda@gmail.com>
 *
 * This file is part of lambda_nixie_clock.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef LAMBDA_NIXIE_CLOCK_COMMON_H
#define LAMBDA_NIXIE_CLOCK_COMMON_H

#ifdef __cplusplus
#  include <string>
#  include <cinttypes>

extern "C" {
#else
#  include <inttypes.h>
#endif

#include <stm32f0xx_hal.h>

// The size of the nixie tube array of the clock.
// Even numbers are recommended.
#define NIXIE_COUNT 8
// Allow the blinking of the status LED of the OtterPill.
#define STATUS_LED 1
#define STATUS_LED_GPIO_PORT GPIOB
#define STATUS_LED_GPIO_PIN  GPIO_PIN_13
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

/**
 * Setups the system clock.
 */
void setup_system_clock(void);

void shift_register_pulse(GPIO_TypeDef* gpio, uint8_t pin);

#ifdef __cplusplus
}
#endif

#endif //LAMBDA_NIXIE_CLOCK_COMMON_H
