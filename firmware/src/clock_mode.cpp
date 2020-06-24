/*
 * Copyright © 2020 LambdAurora <aurora42lambda@gmail.com>
 *
 * This file is part of lambda_nixie_clock.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include <clock_mode.hpp>
#include <console.hpp>

void TimeClockMode::update(ClockManager& manager) {
    bool h24 = manager.config().h24;

    auto hour = manager.current_clock().hour;
    // If 12H mode, then fix hour value.
    if (!h24 && hour > 12)
        hour -= 12;

    auto hour_unit = hour % 10;
    hour /= 10;
    auto minute = manager.current_clock().minute;
    auto minute_unit = minute % 10;
    minute /= 10;
    auto second = manager.current_clock().second;
    auto second_unit = second % 10;
    second /= 10;

    auto separator_on = !(second_unit % 2);

    // Reset to avoid display bugs.
    manager.nixies.reset();

    // Straightforward but not very good.
#if NIXIE_COUNT <= 6
    manager.nixies.at(0).number(hour);
    manager.nixies.at(1).number(hour_unit);
    manager.nixies.at(2).number(minute);
    manager.nixies.at(3).number(minute_unit);
    if (manager.nixies.size() == 6) {
        manager.nixies.at(4).number(second);
        manager.nixies.at(5).number(second_unit);
    }
#else
    // Align to center if there's more than 8 nixie tubes.
    size_t start = NIXIE_COUNT / 2 - 4;
    manager.nixies.at(start).number(hour);
    manager.nixies.at(start + 1).number(hour_unit);
    manager.nixies.at(start + 3).number(minute);
    manager.nixies.at(start + 4).number(minute_unit);
    manager.nixies.at(start + 6).number(second);
    manager.nixies.at(start + 7).number(second_unit);
    for (size_t i = start + 2; i <= start + 5; i += 3) {
        manager.nixies.at(i).left_comma(separator_on);
        manager.nixies.at(i).right_comma(separator_on);
    }
#endif

    manager.nixies.display(false);
}

ClockManager::ClockManager(const DS3231& rtc, const Configuration& config, std::string default_name, ClockMode* default_mode) : _default(std::move(default_name)),
                                                                                                                                _rtc(rtc), _config_manager(config) {
    register_mode(this->_default, default_mode);
    this->_current_mode = this->_default;
    this->_config_manager.read(&this->_config);
}

void ClockManager::register_mode(const std::string& name, ClockMode* mode) {
    this->modes[name] = mode;
}

ClockMode* ClockManager::get_mode(const std::string& name) {
    if (!this->modes.count(name))
        return this->modes[this->_default];
    return this->modes[name];
}

void ClockManager::update() {
    this->_rtc.get_time(&this->_current_clock);
    if (this->_last_clock.year == 0)
        this->_last_clock = this->_current_clock;

    // Blink the status LED every seconds if enabled.
#if STATUS_LED == 1
    if (this->_current_clock.second % 2) {
        HAL_GPIO_WritePin(STATUS_LED_GPIO_PORT, STATUS_LED_GPIO_PIN, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(STATUS_LED_GPIO_PORT, STATUS_LED_GPIO_PIN, GPIO_PIN_RESET);
    }
#endif

    ClockMode* mode = this->get_mode(this->_current_mode);

    mode->update(*this);

    this->_last_clock = this->_current_clock;
}

DS3231& ClockManager::rtc() {
    return this->_rtc;
}

Config& ClockManager::config() {
    return this->_config;
}

Configuration& ClockManager::config_manager() {
    return this->_config_manager;
}

rtc_t& ClockManager::current_clock() {
    return this->_current_clock;
}
