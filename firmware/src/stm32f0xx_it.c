/*
 * Copyright © 2020 LambdAurora <aurora42lambda@gmail.com>
 *
 * This file is part of lambda_nixie_clock.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include <main.h>
#include <stm32f0xx_it.h>

void NMI_Handler(void) {}

void HardFault_Handler(void) {
    while (1) {}
}

void SVC_Handler(void) {}

void PendSV_Handler(void) {}

void SysTick_Handler(void) {
    HAL_IncTick();
}

void USARTx_IRQHandler(void)
{

}
