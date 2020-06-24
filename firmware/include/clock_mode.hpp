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
    void update(ClockManager& manager) override;
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

public:
    NixieArray nixies;

    explicit ClockManager(const DS3231& rtc, const Configuration& config, std::string default_name, ClockMode* default_mode);

    void register_mode(const std::string& name, ClockMode* mode);

    ClockMode* get_mode(const std::string& name);

    void update();

    DS3231& rtc();

    Config& config();

    Configuration& config_manager();

    rtc_t& current_clock();
};

#endif //LAMBDA_NIXIE_CLOCK_CLOCK_MODE_HPP
