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

void TimeClockMode::init(ClockManager& manager) {}

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

    // Straightforward but not very good.
#if NIXIE_COUNT <= 6
    manager.nixies.at(0).number(hour);
    manager.nixies.at(1).number(hour_unit);
    manager.nixies.at(2).number(minute);
    manager.nixies.at(3).number(minute_unit);
#  if NIXIE_COUNT == 6
        manager.nixies.at(4).number(second);
        manager.nixies.at(5).number(second_unit);
#  endif
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

void DateClockMode::init(ClockManager& manager) {
    auto full_year = manager.config().date_full_year;
    auto date_format = manager.config().date_format;

#if NIXIE_COUNT <= 6
    full_year = false;
#endif

    // Indexes array for display format from _values array.
    // Out-of-bound special values:
    // 8: represents a separator if 8 or more nixie tubes are available, else ignored.
    // 9: ignored.
    // 10: ignored if 8 or less nixie tubes are available, else represents a separator.
    switch (date_format) {
        case DATE_DMY:
            if (full_year) this->_indexes = {0, 1, 10, 2, 3, 10, 4, 5, 6, 7};
            else this->_indexes = {0, 1, 8, 2, 3, 8, 6, 7, 9, 9};
            break;
        case DATE_YMD:
#if NIXIE_COUNT > 6
            if (full_year) this->_indexes = {4, 5, 6, 7, 10, 2, 3, 10, 0, 1};
            else this->_indexes = {6, 7, 8, 2, 3, 8, 0, 1, 9, 9};
#elif NIXIE_COUNT == 6
        this->_indexes = {6, 7, 2, 3, 0, 1, 8, 8, 8, 8};
#else
        this->_indexes = {2, 3, 0, 1, 8, 8, 8, 8, 8, 8};
#endif
            break;
        case DATE_MDY:
            if (full_year) this->_indexes = {2, 3, 10, 0, 1, 10, 4, 5, 6, 7};
            else this->_indexes = {2, 3, 8, 0, 1, 8, 6, 7, 9, 9};
            break;
    }

    // If more than 8 nixie tubes are available, then center the date display.
#if NIXIE_COUNT > 8
    this->nixie_start = NIXIE_COUNT / 2 - (full_year ? 10 : 8) / 2;
#endif
}

void DateClockMode::update(ClockManager& manager) {
    auto separator_on = !(manager.current_clock().second % 2);

    // Day of month
    this->_values[0] = manager.current_clock().day_of_month;
    this->_values[1] = this->_values[0] % 10;
    this->_values[0] /= 10;

    // Month
    this->_values[2] = manager.current_clock().month;
    this->_values[3] = this->_values[2] % 10;
    this->_values[2] /= 10;

    // Year
    auto year = manager.current_clock().year;
    for (size_t i = 7; i > 3; i--) {
        this->_values[i] = year % 10;
        year /= 10;
    }

    size_t nixie_index = this->nixie_start;
    for (auto date_index : this->_indexes) {
        if (nixie_index > NIXIE_COUNT)
            break;

        if (date_index == 8) {
#if NIXIE_COUNT < 8
            // Ignore
            continue;
#else
            // Separator
            manager.nixies.at(nixie_index).left_comma(separator_on);
            manager.nixies.at(nixie_index).right_comma(separator_on);
#endif
        } else if (date_index == 9) {
            // Ignore
            continue;
        } else if (date_index == 10) {
#if NIXIE_COUNT > 8
            // Separator
            manager.nixies.at(nixie_index).left_comma(separator_on);
            manager.nixies.at(nixie_index).right_comma(separator_on);
#else
            // Ignore
            continue;
#endif
        } else
            manager.nixies.at(nixie_index).number(this->_values[date_index]);

        nixie_index++;
    }

    manager.nixies.display(true);
}

uint32_t DateClockMode::timeout() {
    return 60;
}

ClockManager::ClockManager(const DS3231& rtc, const Configuration& config, std::string default_name, ClockMode* default_mode) : _default(std::move(default_name)),
                                                                                                                                _rtc(rtc), _config_manager(config) {
    register_mode(this->_default, default_mode);
    this->reload_config();
    this->set_current_mode(this->_default);
}

void ClockManager::register_mode(const std::string& name, ClockMode* mode) {
    this->modes[name] = mode;
}

ClockMode* ClockManager::get_mode(const std::string& name) {
    if (!this->modes.count(name))
        return this->modes[this->_default];
    return this->modes[name];
}

bool ClockManager::set_current_mode(const std::string& name) {
    if (!this->modes.count(name))
        return false;
    this->_current_mode = name;
    auto mode = this->get_mode(name);
    if (mode == nullptr) // That should not be possible.
        return false;
    mode->init(*this);

    this->last_interaction = HAL_GetTick();
    return true;
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

    if (mode->timeout()) {
        uint32_t current_time = HAL_GetTick();
        auto diff_time = current_time - last_interaction;

        if (mode->timeout() < (diff_time / 1000)) {
            this->set_current_mode(this->_default);
            mode = this->get_mode(this->_default);
        }
    }

    // Reset to avoid display bugs.
    this->nixies.reset();
    mode->update(*this);

    this->_last_clock = this->_current_clock;
}

DS3231& ClockManager::rtc() {
    return this->_rtc;
}

Config& ClockManager::config() {
    return this->_config;
}

void ClockManager::reload_config() {
    this->_config_manager.read(&this->_config);
}

Configuration& ClockManager::config_manager() {
    return this->_config_manager;
}

rtc_t& ClockManager::current_clock() {
    return this->_current_clock;
}
