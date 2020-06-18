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
extern "C" {
#endif

#include <stm32f0xx_hal.h>

/**
 * Setups the system clock.
 */
void setup_system_clock(void);

#ifdef __cplusplus
}
#endif

#endif //LAMBDA_NIXIE_CLOCK_COMMON_H
