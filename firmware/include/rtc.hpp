/*
 * Copyright © 2020 LambdAurora <aurora42lambda@gmail.com>
 *
 * This file is part of lambda_nixie_clock.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef LAMBDA_NIXIE_CLOCK_RTC_HPP
#define LAMBDA_NIXIE_CLOCK_RTC_HPP

#define DS3231_ADDRESS (0x68 << 1)   ///< I2C address for DS3231.
#define DS3231_TIME_REGISTER 0x00      ///< Time register
#define DS3231_ALARM1_REG 0x07    ///< Alarm 1 register
#define DS3231_ALARM2_REG 0x0B    ///< Alarm 2 register
#define DS3231_CONTROL_REG 0x0E   ///< Control register
#define DS3231_STATUS_REG 0x0F ///< Status register
#define DS3231_TEMPERATURE_REG 0x11 ///< Temperature register (high byte - low byte is at 0x12), 10-bit.

#include <common.h>
#include <stm32f0xx_hal_i2c.h>
#include <cinttypes>

/**
 * Represents the real time clock.
 */
typedef struct
{
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day_of_week;
    uint8_t day_of_month;
    uint8_t month;
    uint32_t year;
} rtc_t;

enum Alarm : uint8_t {
    ALARM_1 = 0,
    ALARM_2 = 1
};

enum DS3231Alarm1Mode : uint8_t {
    DS3231_A1_PER_SECOND = 0x0F,
    DS3231_A1_SECOND = 0x0E,
    DS3231_A1_MINUTE = 0x0C,
    DS3231_A1_HOUR = 0x08,
    DS3231_A1_DATE = 0x00,
    DS3231_A1_DAY = 0x10
};

enum DS3231Alarm2Mode : uint8_t {
    DS3231_A2_PER_MINUTE = 0x07,
    DS3231_A2_MINUTE = 0x06,
    DS3231_A2_HOUR = 0x04,
    DS3231_A2_DATE = 0x00,
    DS3231_A2_DAY = 0x08
};

class DS3231
{
private:
    I2C_HandleTypeDef i2ch = {nullptr};

    uint8_t read_status_register();

    uint8_t read_control_register();

public:
    DS3231();

    explicit DS3231(I2C_HandleTypeDef i2c_handle);

    void begin(I2C_HandleTypeDef i2c_handle);

    bool is_available();

    /**
     * Returns whether the DS3231 has lost power or not.
     * @return True if the DS3231 has lost power, else false.
     */
    bool has_lost_power();

    void enable_oscillator();

    /**
     * Sets the time of the DS3231.
     * @param clock The time.
     */
    void set_time(const rtc_t& clock);

    /**
     * Gets the time from the DS3231.
     * @param clock The time.
     */
    void get_time(rtc_t* clock);

    /**
     * Gets the temperature from the DS3231.
     * @return The temperature in Celsius.
     */
    float get_temp();

    void force_temp_conv();

    bool set_alarm1(const rtc_t& clock, DS3231Alarm1Mode mode);

    bool set_alarm2(const rtc_t& clock, DS3231Alarm2Mode mode);

    bool reset_alarm(Alarm alarm);

    /**
     * Disables the specified alarm.
     * @param alarm The alarm to disable.
     */
    bool disable_alarm(uint8_t alarm);

    /**
     * Clears the status of the specified alarm.
     * @param alarm The alarm to clear.
     */
    bool clear_alarm(uint8_t alarm);

    /**
     * Returns whether the specified alarm has fired.
     * @param alarm The alarm to check.
     * @return True if the alarm has fired, else false.
     */
    bool alarm_fired(uint8_t alarm);
};

#endif //LAMBDA_NIXIE_CLOCK_RTC_HPP
