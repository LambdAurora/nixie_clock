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
#include <random>

// Random generator.
std::default_random_engine generator;
std::uniform_int_distribution<int> distribution(0, 9);

/* Time Clock Mode */

std::string TimeClockMode::name() const {
    return "time";
}

void TimeClockMode::update(ClockManager& manager) {
    bool h24 = manager.config().h24;

    auto hour = manager.current_clock().hour;
    // If 12H mode, then fix hour value.
    if (!h24 && hour > 12)
        hour -= 12;

    this->_values[1] = hour % 10;
    this->_values[0] = hour / 10;
    this->_values[2] = manager.current_clock().minute;
    this->_values[3] = this->_values[2] % 10;
    this->_values[2] /= 10;
    this->_values[4] = manager.current_clock().second;
    this->_values[5] = this->_values[4] % 10;
    this->_values[4] /= 10;

    auto separator_on = !(this->_values[5] % 2);

    // Straightforward but not very good.
#if NIXIE_COUNT <= 6
    size_t start = 0;
    if (this->_values[4] % 2)
        start = 2;
    size_t nixie_index = 0;
    for (size_t i = start; i < 6; i++) {
        if (nixie_index >= manager.nixies.size())
            break;

        manager.nixies.at(nixie_index).number(this->_values[i]);

#if NIXIE_COUNT == 2
        if (start == 0)
            manager.nixies.at(nixie_index).right_comma(separator_on);
        else
            manager.nixies.at(nixie_index).left_comma(separator_on);
#endif

        nixie_index++;
    }
#else
    // Align to center if there's more than 8 nixie tubes.
    size_t start = NIXIE_COUNT / 2 - 4;
    manager.nixies.at(start).number(this->_values[0]);
    manager.nixies.at(start + 1).number(this->_values[1]);
    manager.nixies.at(start + 3).number(this->_values[2]);
    manager.nixies.at(start + 4).number(this->_values[3]);
    manager.nixies.at(start + 6).number(this->_values[4]);
    manager.nixies.at(start + 7).number(this->_values[5]);
    for (size_t i = start + 2; i <= start + 5; i += 3) {
        manager.nixies.at(i).left_comma(separator_on);
        manager.nixies.at(i).right_comma(separator_on);
    }
#endif

    manager.nixies.display(NIXIE_COMMAS);
}

/* Date Clock Mode */

std::string DateClockMode::name() const {
    return "date";
}

void DateClockMode::init(ClockManager& manager) {
    auto full_year = manager.config().date_full_year;
    auto date_format = manager.config().date_format;

#if NIXIE_COUNT <= 6
    full_year = false;
#endif

    // Indexes array for display format from _values array.
    // Out-of-bound special values:
    // 8: represents a separator if 8 or< more nixie tubes are available, else ignored.
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

    manager.nixies.display(NIXIE_COMMAS);
}

uint32_t DateClockMode::timeout() const {
    return 60;
}

/* Thermometer Clock Mode */

std::string ThermometerClockMode::name() const {
    return "thermometer";
}

void ThermometerClockMode::update(ClockManager& manager) {
    auto compact = manager.config().compact_temperature;
    auto temp = static_cast<int32_t>(manager.rtc().get_temp() * 100);

    for (size_t i = 4; i > 0; i--) {
        this->_values[i - 1] = temp % 10;
        temp /= 10;
    }

    size_t nixie_index = NIXIE_COUNT - (compact ? 4 : 5);
    if (manager.nixies.size() == 2)
        nixie_index = 0;

    for (size_t i = 0; i < 4; i++) {
        if (nixie_index >= NIXIE_COUNT)
            break;
        if (i == 0 && this->_values[i] == 0)
            continue;

        manager.nixies.at(nixie_index).number(this->_values[i]);
        if (i == 1) {
            if (compact)
                manager.nixies.at(nixie_index).right_comma(true);
            else {
                nixie_index++;
                manager.nixies.at(nixie_index).right_comma(true);
            }
        }
        nixie_index++;
    }

    manager.nixies.display(NIXIE_COMMAS);
}

uint32_t ThermometerClockMode::timeout() const {
    return 60;
}

/* Random Clock Mode */
// Cool effect, for debugging.
#if DEBUG_MODE
std::string RandomClockMode::name() const {
    return "random";
}

void RandomClockMode::init(ClockManager &manager) {
    for (size_t i = 0; i < NIXIE_COUNT; i++) {
        this->_values[i] = distribution(generator);
    }
    this->_displayed = 0;
}

void RandomClockMode::update(ClockManager &manager) {
    if (this->_displayed >= 50)
        init(manager);

    for (size_t i = 0; i < manager.nixies.size(); i++) {
        manager.nixies.at(i).number(this->_values[i]);
    }

    manager.nixies.display(NIXIE_COMMAS);

    this->_displayed++;
}

uint32_t RandomClockMode::timeout() const {
    return 120;
}
#endif

/* Clock Manager */

ClockManager::ClockManager(const DS3231& rtc, const Configuration& config, ClockMode* default_mode) : _rtc(rtc), _config_manager(config) {
    register_mode(default_mode);
    this->reload_config();
    this->set_current_mode(default_mode->name());
    this->_default = default_mode->name();
    this->_config.cathode_poisoning_cycle = CPC_30M;

    this->prevent_cathode_poisoning();
}

void ClockManager::register_mode(ClockMode* mode) {
    this->modes.push_back(mode);
}

ClockMode* ClockManager::get_mode(const std::string& name) {
    for (auto mode : this->modes) {
        if (mode->name() == name)
            return mode;
    }
    return this->modes[0];
}

bool ClockManager::set_current_mode(const std::string& name) {
    for (size_t i = 0; i < this->modes.size(); i++) {
        auto mode = this->modes[i];
        if (mode->name() == name) {
            this->_current_mode = i;
            mode->init(*this);

            this->last_interaction = HAL_GetTick();
            return true;
        }
    }
    return false;
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

    bool mode_button = HAL_GPIO_ReadPin(BUTTON_MODE_GPIO_PORT, BUTTON_MODE_GPIO_PIN);
    if (mode_button && mode_button != this->_last_mode_button_value) {
        this->cycle_mode();
    }

    ClockMode* mode = this->modes[this->_current_mode];

    bool cathode_poisoning_cycle = false;

    if (mode->timeout()) {
        uint32_t current_time = HAL_GetTick();
        auto diff_time = current_time - last_interaction;

        if (mode->timeout() < (diff_time / 1000)) {
            this->set_current_mode(this->_default);
            mode = this->get_mode(this->_default);
        }
    } else {
        if (this->_config.cathode_poisoning_cycle > 100 && this->_current_clock.minute == 0 && this->_current_clock.second < 30) {
            uint8_t cycle_hour = this->_config.cathode_poisoning_cycle - 100;
            if (!(this->_current_clock.hour % cycle_hour))
                cathode_poisoning_cycle = true;
        } else if (!(this->_current_clock.minute % this->_config.cathode_poisoning_cycle) && this->_current_clock.second < 30)
            cathode_poisoning_cycle = true;
    }

    // Reset to avoid display bugs.
    this->nixies.reset();
    if (!cathode_poisoning_cycle) {
        mode->update(*this);
    } else
        this->prevent_cathode_poisoning();

    this->_last_clock = this->_current_clock;
    this->_last_mode_button_value = mode_button;
}

void ClockManager::prevent_cathode_poisoning() {
    this->nixies.reset();

    uint32_t delay = 500;

    for (size_t nixie = 0; nixie < this->nixies.size(); nixie++)
        this->nixies.at(nixie).left_comma(true);
    this->nixies.display(NIXIE_COMMAS);

    // Delays are bad but in this case we don't want anything to interrupt this.
    HAL_Delay(delay);
    this->nixies.reset();

    for (uint8_t i = 0; i < 10; i++) {
        for (size_t nixie = 0; nixie < this->nixies.size(); nixie++)
            this->nixies.at(nixie).number(i);
        this->nixies.display(NIXIE_COMMAS);
        HAL_Delay(delay);
    }
    this->nixies.reset();

    for (size_t nixie = 0; nixie < this->nixies.size(); nixie++)
        this->nixies.at(nixie).right_comma(true);
    this->nixies.display(NIXIE_COMMAS);

    HAL_Delay(delay);
    this->nixies.reset();
}


void ClockManager::count_to_100() {
    for (uint8_t i = 0; i < 100; i++) {
        this->nixies.reset();
        uint8_t unit = i % 10;
        uint8_t ten = i / 10;
        for (size_t nixie = 0; nixie < this->nixies.size(); nixie++) {
            this->nixies.at(nixie).number(!(nixie % 2) ? ten : unit);
        }
        this->nixies.display(NIXIE_COMMAS);
        HAL_Delay(50);
    }
}

void ClockManager::cycle_mode() {
    bool found = false;
    auto current_mode = this->modes[_current_mode];
    for (auto mode : this->modes) {
        if (current_mode == mode) {
            found = true;
        } else if (found) {
            this->set_current_mode(mode->name());
            found = false;
            break;
        }
    }

    if (found) {
        this->set_current_mode(this->_default);
    }
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
