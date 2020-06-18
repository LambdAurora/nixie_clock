/*
 * Copyright © 2020 LambdAurora <aurora42lambda@gmail.com>
 *
 * This file is part of lambda_nixie_clock.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include <ee24.hpp>

EE24::EE24(uint16_t address) : dev_addr(address) {}

EE24::EE24(uint16_t address, I2C_HandleTypeDef i2c_handle) : dev_addr(address), i2ch(i2c_handle) {}

void EE24::begin(I2C_HandleTypeDef i2c_handle) {
    this->i2ch = i2c_handle;
}

bool EE24::is_available() {
    return this->i2ch.Instance != nullptr && HAL_I2C_IsDeviceReady(&this->i2ch, this->dev_addr, 2, 100) == HAL_OK;
}

bool EE24::read(uint16_t address, uint8_t* buffer, uint16_t size) {
    if (this->lock || !this->is_available())
        return false;

    this->lock = true;
    HAL_StatusTypeDef result = HAL_I2C_Mem_Read(&this->i2ch, this->dev_addr, address, I2C_MEMADD_SIZE_16BIT, buffer, size, 100);
    this->lock = false;
    return result == HAL_OK;
}

bool EE24::write(uint16_t address, uint8_t* buffer, uint16_t size) {
    if (this->lock || !this->is_available())
        return false;

    this->lock = true;
    HAL_StatusTypeDef result = HAL_I2C_Mem_Write(&this->i2ch, this->dev_addr, address, I2C_MEMADD_SIZE_16BIT, buffer, size, 100);
    this->lock = false;
    return result == HAL_OK;
}
