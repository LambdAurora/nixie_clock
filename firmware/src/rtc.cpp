/*
 * Copyright © 2020 LambdAurora <aurora42lambda@gmail.com>
 *
 * This file is part of lambda_nixie_clock.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include <rtc.hpp>

/**
 * Convert normal decimal numbers to binary coded decimal.
 */
uint8_t int_to_bcd(int32_t val) {
    return (uint8_t) ((val / 10 * 16) + (val % 10));
}

/**
 * Convert binary coded decimal to normal decimal numbers.
 */
int32_t bcd_to_int(uint8_t val) {
    return (val / 16 * 10) + (val % 16);
}

DS3231::DS3231() = default;

DS3231::DS3231(I2C_HandleTypeDef i2c_handle) : i2ch(i2c_handle) {}

void DS3231::begin(I2C_HandleTypeDef i2c_handle) {
    this->i2ch = i2c_handle;
}

bool DS3231::is_available() {
    return this->i2ch.Instance != nullptr && HAL_I2C_IsDeviceReady(&this->i2ch, DS3231_ADDRESS, 2, 100) == HAL_OK;
}

uint8_t DS3231::read_status_register() {
    if (!this->is_available())
        return 0;

    uint8_t status = 0;
    HAL_I2C_Mem_Read(&this->i2ch, DS3231_ADDRESS, DS3231_STATUS_REG, 1, &status, 1, 100);
    return status;
}

uint8_t DS3231::read_control_register() {
    if (!this->is_available())
        return 0;

    uint8_t control = 0;
    HAL_I2C_Mem_Read(&this->i2ch, DS3231_ADDRESS, DS3231_CONTROL_REG, 1, &control, 1, 100);
    return control;
}

bool DS3231::has_lost_power() {
    if (!this->is_available())
        return false;

    return ((this->read_status_register()) >> 7);
}

void DS3231::enable_oscillator() {
    if (!this->is_available())
        return;

    uint8_t status = read_status_register();
    status &= ~0x80;
    HAL_I2C_Mem_Write(&this->i2ch, DS3231_ADDRESS, DS3231_STATUS_REG, 1, &status, 1, 100);
}

void DS3231::set_time(const rtc_t& clock) {
    if (!this->is_available())
        return;

    uint8_t year = 0;
    if (clock.year >= 100) {
        year = year % 100;
    } else {
        year = clock.year;
    }

    uint8_t set_time[7];
    set_time[0] = int_to_bcd(clock.second);
    set_time[1] = int_to_bcd(clock.minute);
    set_time[2] = int_to_bcd(clock.hour);
    set_time[3] = int_to_bcd(clock.day_of_week);
    set_time[4] = int_to_bcd(clock.day_of_month);
    set_time[5] = int_to_bcd(clock.month); // No century bit as it's kind of stupid. (cf century bit comment in get_time)
    set_time[6] = int_to_bcd(year);

    HAL_I2C_Mem_Write(&this->i2ch, DS3231_ADDRESS, DS3231_TIME_REGISTER, 1, set_time, 7, 1000);
    this->enable_oscillator();
}

void DS3231::get_time(rtc_t* clock) {
    if (!this->is_available())
        return;

    uint8_t get_time[7];
    HAL_I2C_Mem_Read(&this->i2ch, DS3231_ADDRESS, DS3231_TIME_REGISTER, 1, get_time, 7, 1000);

    uint8_t century = (get_time[5] & 0x80) >> 7;
    uint8_t month = bcd_to_int(get_time[5] & 0x1F);

    clock->second = bcd_to_int(get_time[0]);
    clock->minute = bcd_to_int(get_time[1]);
    clock->hour = bcd_to_int(get_time[2]);
    clock->day_of_week = bcd_to_int(get_time[3]);
    clock->day_of_month = bcd_to_int(get_time[4]);
    clock->month = month;
    // We assume that we are in the 21th century or the 22th century if the century bit is set to 1.
    // For the 23th century this will need a firmware update, but the hardware won't work anymore sadly.
    // This is also stupid because leap-year compensation will not work for the 22th century.
    // And using it to determine if it's the 20th century is even more stupid as we don't travel back in time...
    clock->year = 2000 + bcd_to_int(get_time[6]) + century * 100;
}

float DS3231::get_temp() {
    if (!this->is_available())
        return 0.f;

    uint8_t temp[2];
    HAL_I2C_Mem_Read(&this->i2ch, DS3231_ADDRESS, DS3231_TEMPERATURE_REG, 1, temp, 2, 1000);

    return static_cast<float>(((temp[0]) + (temp[1] >> 6) / 4.0));
}

void DS3231::force_temp_conv() {
    if (!this->is_available())
        return;

    uint8_t status = this->read_status_register();
    if (!(status & 0x04)) {
        uint8_t control = this->read_control_register();
        HAL_I2C_Mem_Write(&this->i2ch, DS3231_ADDRESS, DS3231_CONTROL_REG, 1, (uint8_t*) (control | 0x20), 1, 100);
    }
}

bool DS3231::set_alarm1(const rtc_t& clock, DS3231Alarm1Mode mode) {
    if (!this->is_available())
        return false;

    uint8_t control = this->read_control_register();
    if (!(control & 0x04))
        return false;

    uint8_t a1m1 = (mode & 0x01) << 7;
    uint8_t a1m2 = (mode & 0x02) << 6;
    uint8_t a1m3 = (mode & 0x04) << 5;
    uint8_t a1m4 = (mode & 0x08) << 4;
    uint8_t dy_dt = (mode & 0x10) << 2;

    uint8_t alarm1[4];
    alarm1[0] = int_to_bcd(clock.second) | a1m1;
    alarm1[1] = int_to_bcd(clock.minute) | a1m2;
    alarm1[2] = int_to_bcd(clock.hour) | a1m3;
    if (dy_dt) {
        alarm1[3] = int_to_bcd(clock.day_of_week) | a1m4 | dy_dt;
    } else {
        alarm1[3] = int_to_bcd(clock.day_of_month) | a1m4 | dy_dt;
    }

    if (HAL_I2C_Mem_Write(&this->i2ch, DS3231_ADDRESS, DS3231_ALARM1_REG, 1, alarm1, 4, 1000) != HAL_OK)
        return false;

    control |= 0x01; // AI1E
    return HAL_I2C_Mem_Write(&this->i2ch, DS3231_ADDRESS, DS3231_CONTROL_REG, 1, &control, 1, 1000) == HAL_OK;
}

bool DS3231::set_alarm2(const rtc_t& clock, DS3231Alarm2Mode mode) {
    if (!this->is_available())
        return false;

    uint8_t control = this->read_control_register();
    if (!(control & 0x04))
        return false;

    uint8_t a2m2 = (mode & 0x01) << 7;
    uint8_t a2m3 = (mode & 0x02) << 6;
    uint8_t a2m4 = (mode & 0x04) << 5;
    uint8_t dy_dt = (mode & 0x08) << 3;

    uint8_t alarm2[3];
    alarm2[0] = int_to_bcd(clock.minute) | a2m2;
    alarm2[1] = int_to_bcd(clock.hour) | a2m3;
    if (dy_dt) {
        alarm2[2] = int_to_bcd(clock.day_of_week) | a2m4 | dy_dt;
    } else {
        alarm2[2] = int_to_bcd(clock.day_of_month) | a2m4 | dy_dt;
    }

    if (HAL_I2C_Mem_Write(&this->i2ch, DS3231_ADDRESS, DS3231_ALARM2_REG, 1, alarm2, 3, 1000) != HAL_OK)
        return false;

    control |= 0x02; // AI2E
    return HAL_I2C_Mem_Write(&this->i2ch, DS3231_ADDRESS, DS3231_CONTROL_REG, 1, &control, 1, 1000) == HAL_OK;
}

bool DS3231::reset_alarm(Alarm alarm) {
    if (!this->is_available())
        return false;

    uint8_t alarm_data[4];
    alarm_data[0] = 0;
    alarm_data[1] = 0;
    alarm_data[2] = 0;
    alarm_data[3] = 0;

    if (HAL_I2C_Mem_Write(&this->i2ch, DS3231_ADDRESS, alarm == ALARM_1 ? DS3231_ALARM1_REG : DS3231_ALARM2_REG, 1, alarm_data, alarm == ALARM_1 ? 4 : 3, 1000) != HAL_OK)
        return false;

    return this->disable_alarm(alarm);
}

bool DS3231::disable_alarm(uint8_t alarm) {
    if (!this->is_available())
        return false;

    uint8_t control = this->read_control_register();
    control &= ~(0x01 << alarm);
    return HAL_I2C_Mem_Write(&this->i2ch, DS3231_ADDRESS, DS3231_CONTROL_REG, 1, &control, 1, 100) == HAL_OK;
}

bool DS3231::clear_alarm(uint8_t alarm) {
    if (!this->is_available())
        return false;

    uint8_t status = this->read_status_register();
    status &= ~(0x01 << alarm);
    return HAL_I2C_Mem_Write(&this->i2ch, DS3231_ADDRESS, DS3231_STATUS_REG, 1, &status, 1, 100) == HAL_OK;
}

bool DS3231::alarm_fired(uint8_t alarm) {
    if (!this->is_available())
        return false;

    uint8_t status = this->read_status_register();
    return (status >> alarm) & 0x01;
}
