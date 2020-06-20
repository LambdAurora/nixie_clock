/*
 * Copyright © 2020 LambdAurora <aurora42lambda@gmail.com>
 *
 * This file is part of lambda_nixie_clock.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include <main.h>
#include <rtc.hpp>
#include <ee24.hpp>

#define LED_PIN GPIO_PIN_13
#define INVERT_STATE(state) (state == GPIO_PIN_RESET ? GPIO_PIN_SET : GPIO_PIN_RESET)

#define INIT_OUTPUT_GPIO(PIN, PORT) {\
    HAL_GPIO_WritePin(PORT, PIN, GPIO_PIN_RESET);\
    \
    GPIO_InitStruct.Pin = PIN;\
    GPIO_InitStruct.Pin = GPIO_MODE_OUTPUT_PP;\
    GPIO_InitStruct.Pull = GPIO_NOPULL;\
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;\
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
    HAL_GPIO_WritePin(GPIOB, LED_PIN, GPIO_PIN_RESET);

    // HAL GPIO initialization is a bit long honestly.
    GPIO_InitStruct.Pin = LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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
    auto eeprom = EE24(EE24_ADDRESS, i2c1h);

    ds3231.enable_oscillator();

    rtc_t clock;

    ds3231.reset_alarm(ALARM_2);

    GPIO_PinState state = GPIO_PIN_RESET;
    while (true) {
        ds3231.get_time(&clock);
        ds3231.force_temp_conv();
        float temp = ds3231.get_temp();
        console::cout.writef("DS3231 ; lost_power = %d ; temp = %d°C\r\n", ds3231.has_lost_power(), static_cast<int>(temp * 100));
        console::cout.writef("%d:%d:%d %d/%d/%d\r\n", clock.hour, clock.minute, clock.second, clock.day_of_month, clock.month, clock.year);

        state = INVERT_STATE(state);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, state);

        HAL_Delay(500);
    }

    return 0;
}
