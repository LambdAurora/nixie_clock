/*
 * Copyright © 2020 LambdAurora <aurora42lambda@gmail.com>
 *
 * This file is part of lambda_nixie_clock.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef __STM32F0xx_HAL_CONF_H
#define __STM32F0xx_HAL_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* ======================== Modules selection ======================== */

#define HAL_MODULE_ENABLED

#define HAL_CORTEX_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_EXTI_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_I2C_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED
#define HAL_USART_MODULE_ENABLED

/* ==================== HSE/HSI Values adaptation ==================== */

#ifndef HSE_VALUE
#define HSE_VALUE ((uint32_t) 8000000) /* Value of the External oscillator in Hz. */
#endif

#ifndef HSE_STARTUP_TIMEOUT
#define HSE_STARTUP_TIMEOUT ((uint32_t) 100) /* Time out for HSE start up, in ms. */
#endif

#ifndef HSI_VALUE
#define HSI_VALUE ((uint32_t) 8000000) /* Value of the Internal oscillator in Hz. */
#endif

#ifndef HSI_STARTUP_TIMEOUT
#define HSI_STARTUP_TIMEOUT ((uint32_t) 5000) /* Time out for HSI start up, in ms. */
#endif

#ifndef HSI14_VALUE
#define HSI14_VALUE ((uint32_t) 14000000) /* Value of the Internal High Speed oscillator for ADC in Hz.
                                             The real value may vary depending on the variations
                                             in voltage and temperature. */
#endif

#ifndef HSI48_VALUE
#define HSI48_VALUE ((uint32_t) 48000000) /* Value of the Internal High Speed oscillator for USB in Hz.
                                             The real value may vary depending on the variations
                                             in voltage and temperature. */
#endif

#ifndef LSI_VALUE
#define LSI_VALUE ((uint32_t) 40000) /* Value of the Internal Low Speed oscillator in Hz
                                        The real value may vary depending on the variations
                                        in voltage and temperature.  */
#endif

#ifndef LSE_VALUE
#define LSE_VALUE ((uint32_t) 32768) /* Value of the External Low Speed oscillator in Hz. */
#endif

#ifndef LSE_STARTUP_TIMEOUT
#define LSE_STARTUP_TIMEOUT ((uint32_t)5000) /* Time out for LSE start up, in ms. */
#endif

/* ====================== System configuration ======================= */

#define VDD_VALUE ((uint32_t) 3300) /* Value of VDD in mV. */
#define TICK_INT_PRIORITY ((uint32_t) 0)
#define USE_RTOS 0
#define PREFETCH_ENABLE 1
#define INSTRUCTION_CACHE_ENABLE  0
#define DATA_CACHE_ENABLE 0
#define USE_SPI_CRC  0U

#define USE_HAL_I2C_REGISTER_CALLBACKS   0U /* I2C register callback disabled. */
#define USE_HAL_UART_REGISTER_CALLBACKS  0U /* UART register callback disabled. */
#define USE_HAL_USART_REGISTER_CALLBACKS 0U /* USART register callback disabled. */

/* ============================ Includes ============================= */

#ifdef HAL_CORTEX_MODULE_ENABLED
#include <stm32f0xx_hal_cortex.h>
#endif

#ifdef HAL_DMA_MODULE_ENABLED
#include <stm32f0xx_hal_dma.h>
#endif

#ifdef HAL_EXTI_MODULE_ENABLED
#include <stm32f0xx_hal_exti.h>
#endif

#ifdef HAL_GPIO_MODULE_ENABLED
#include <stm32f0xx_hal_gpio.h>
#endif

#ifdef HAL_FLASH_MODULE_ENABLED
#include <stm32f0xx_hal_flash.h>
#endif

#ifdef HAL_I2C_MODULE_ENABLED
#include <stm32f0xx_hal_i2c.h>
#endif

#ifdef HAL_PWR_MODULE_ENABLED
#include <stm32f0xx_hal_pwr.h>
#endif

#ifdef HAL_RCC_MODULE_ENABLED
#include <stm32f0xx_hal_rcc.h>
#endif

#ifdef HAL_UART_MODULE_ENABLED
#include <stm32f0xx_hal_uart.h>
#endif

#ifdef HAL_USART_MODULE_ENABLED
#include <stm32f0xx_hal_usart.h>
#endif

#define assert_param(expr) ((void) 0U)

#ifdef __cplusplus
}
#endif

#endif // __STM32F0xx_HAL_CONF_H
