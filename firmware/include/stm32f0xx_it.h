/*
 * Copyright © 2020 LambdAurora <aurora42lambda@gmail.com>
 *
 * This file is part of lambda_nixie_clock.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef LAMBDA_NIXIE_CLOCK_STM32F0XX_IT_H
#define LAMBDA_NIXIE_CLOCK_STM32F0XX_IT_H

#ifdef __cplusplus
extern "C" {
#endif

void NMI_Handler(void);

void HardFault_Handler(void);

void SVC_Handler(void);

void PendSV_Handler(void);

void SysTick_Handler(void);

void USARTx_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif //LAMBDA_NIXIE_CLOCK_STM32F0XX_IT_H
