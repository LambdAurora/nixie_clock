/*
 * Copyright © 2020 LambdAurora <aurora42lambda@gmail.com>
 *
 * This file is part of lambda_nixie_clock.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include <main.h>
#include <clock_mode.hpp>

#define INIT_OUTPUT_GPIO(PIN, PORT) {\
    HAL_GPIO_WritePin(PORT, PIN, GPIO_PIN_RESET);\
    \
    GPIO_InitStruct.Pin = PIN;\
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;\
    GPIO_InitStruct.Pull = GPIO_NOPULL;\
    HAL_GPIO_Init(PORT, &GPIO_InitStruct);\
  }

I2C_HandleTypeDef i2c1h;

/**
 * Setup the GPIO.
 */
static void setup_gpio() {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // Let's initialize the blinking LED pin.
    HAL_GPIO_WritePin(STATUS_LED_GPIO_PORT, STATUS_LED_GPIO_PIN, GPIO_PIN_RESET);

    // HAL GPIO initialization is a bit long honestly.
    GPIO_InitStruct.Pin = STATUS_LED_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(STATUS_LED_GPIO_PORT, &GPIO_InitStruct);

    // Buttons
    GPIO_InitStruct.Pin = BUTTON_MODE_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(BUTTON_MODE_GPIO_PORT, &GPIO_InitStruct);

    // Shift register GPIO.
    INIT_OUTPUT_GPIO(SDI_PIN, SDI_GPIO_PORT)
    INIT_OUTPUT_GPIO(RCLK_PIN, RCLK_GPIO_PORT)
    INIT_OUTPUT_GPIO(SRCLK_PIN, SRCLK_GPIO_PORT)

    shift_register_pulse(RCLK_GPIO_PORT, RCLK_PIN);
    shift_register_pulse(SRCLK_GPIO_PORT, SRCLK_PIN);
}

void setup_i2c() {
    i2c1h.Instance = I2C1;
    i2c1h.Init.Timing = 0x50330309;
    i2c1h.Init.OwnAddress1 = 0;
    i2c1h.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    i2c1h.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    i2c1h.Init.OwnAddress2 = 0;
    i2c1h.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    i2c1h.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&i2c1h) != HAL_OK) {
        //Error_Handler();
    }
}

int main() {
    HAL_Init();
    setup_system_clock();

    setup_gpio();
    setup_i2c();
    console::setup();

    auto ds3231 = DS3231(i2c1h);
    if (ds3231.has_lost_power()) {
        // If the RTC module lost power, restore the last time and date the firmware knew.
        // Mmm dd yyyy
        std::string date_str = __DATE__;
        // HH:mm:ss
        std::string time_str = __TIME__;

        const auto ASCII_ZERO = static_cast<uint8_t>('0');

        rtc_t clock;
        clock.hour = (time_str[0] - ASCII_ZERO) * 10 + (time_str[1] - ASCII_ZERO);
        clock.minute = (time_str[3] - ASCII_ZERO) * 10 + (time_str[4] - ASCII_ZERO);
        clock.second = (time_str[6] - ASCII_ZERO) * 10 + (time_str[7] - ASCII_ZERO);

        std::array<std::string, 12> months = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
        for (uint8_t i = 0; i < months.size(); i++) {
            if (date_str.find(months[i]) != std::string::npos)
                clock.month = i + 1;
        }

        auto day_of_month_ten = (date_str[4] - ASCII_ZERO) * 10;
        if (date_str[4] == ' ') day_of_month_ten = 0;
        clock.day_of_month = day_of_month_ten + (date_str[5] - ASCII_ZERO);
        clock.year = (date_str[7] - ASCII_ZERO) * 1000
                + (date_str[8] - ASCII_ZERO) * 100
                + (date_str[9] - ASCII_ZERO) * 10
                + (date_str[10] - ASCII_ZERO);
        clock.day_of_week = get_day_of_week(clock);

        ds3231.set_time(clock);
    }

    auto configuration = Configuration({EE24_ADDRESS, i2c1h});
    if (!configuration.init())
        return 1;
    // @TODO remove reset
    configuration.reset();

    auto nixies = NixieArray();
    nixies.reset();

    ds3231.enable_oscillator();

    TimeClockMode time_clock_mode;
    DateClockMode date_clock_mode;
    ThermometerClockMode thermometer_clock_mode;
    auto clock = ClockManager(ds3231, configuration, &time_clock_mode);
    clock.register_mode(&date_clock_mode);
    clock.register_mode(&thermometer_clock_mode);

    while (true) {
        clock.update();
    }

    return 0;
}
