/*
 * Copyright © 2020 LambdAurora <aurora42lambda@gmail.com>
 *
 * This file is part of lambda_nixie_clock.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef LAMBDA_NIXIE_CLOCK_CLOCK_MODE_HPP
#define LAMBDA_NIXIE_CLOCK_CLOCK_MODE_HPP

#include <rtc.hpp>
#include <nixie.hpp>
#include <config.hpp>
#include <map>

class ClockManager;

/**
 * Represents a clock mode.
 */
class ClockMode {
public:
    /**
     * Initializes the mode.
     * @param manager The clock manager.
     */
    virtual void init(ClockManager& manager) = 0;

    /**
     * Update method executed at each iteration of the main loop.
     * @param manager The clock manager.
     */
    virtual void update(ClockManager& manager) = 0;

    /**
     * Returns the timeout before the clock manager switches back to the default mode.
     * @return 0 to disable the timeout, else a timeout value in seconds.
     */
    virtual uint32_t timeout() {
        return 0;
    }
};

class TimeClockMode : public ClockMode {
public:
    void init(ClockManager& manager) override;

    void update(ClockManager& manager) override;
};

class DateClockMode : public ClockMode {
private:
    std::array<uint8_t, 8> _values{0, 0, 0, 0, 0, 0, 0, 0};
    std::array<size_t, 10> _indexes{8, 8, 8, 8, 8, 8, 8, 8, 8, 8};
    size_t nixie_start = 0;

public:
    void init(ClockManager& manager) override;

    void update(ClockManager& manager) override;

    uint32_t timeout() override;
};

class ClockManager {
private:
    std::map<std::string, ClockMode*> modes;
    std::string _default;
    std::string _current_mode;
    DS3231 _rtc;
    rtc_t _current_clock;
    rtc_t _last_clock;
    Config _config;
    Configuration _config_manager;
    uint32_t last_interaction;

public:
    NixieArray nixies;

    explicit ClockManager(const DS3231& rtc, const Configuration& config, std::string default_name, ClockMode* default_mode);

    void register_mode(const std::string& name, ClockMode* mode);

    ClockMode* get_mode(const std::string& name);

    /**
     * Sets the current mode.
     * @param name The name of the mode.
     * @return True if the current mode was set successfully, else false.
     */
    bool set_current_mode(const std::string& name);

    /**
     * Updates the modes and the RTC clock value, resets the display, etc.
     */
    void update();

    /**
     * Returns the instance of the RTC module.
     * @return The instance of the RTC module.
     */
    DS3231& rtc();

    /**
     * Returns the current configuration.
     * @return The current configuration.
     */
    Config& config();

    /**
     * Reloads the configuration from the EEPROM.
     */
    void reload_config();

    Configuration& config_manager();

    rtc_t& current_clock();
};

#endif //LAMBDA_NIXIE_CLOCK_CLOCK_MODE_HPP
