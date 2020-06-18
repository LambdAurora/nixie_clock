/*
 * Copyright © 2020 LambdAurora <aurora42lambda@gmail.com>
 *
 * This file is part of lambda_nixie_clock.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef LAMBDA_NIXIE_CLOCK_EE24_HPP
#define LAMBDA_NIXIE_CLOCK_EE24_HPP

#include <common.h>
#include <stm32f0xx_hal_i2c.h>

class EE24 {
private:
    uint16_t dev_addr;
    I2C_HandleTypeDef i2ch = { nullptr };
    bool lock = false;

public:
    EE24(uint16_t address);

    EE24(uint16_t address, I2C_HandleTypeDef i2c_handle);

    void begin(I2C_HandleTypeDef i2c_handle);

    bool is_available();

    bool read(uint16_t address, uint8_t* buffer, uint16_t size);

    bool write(uint16_t address, uint8_t* buffer, uint16_t size);
};

#endif //LAMBDA_NIXIE_CLOCK_EE24_HPP
