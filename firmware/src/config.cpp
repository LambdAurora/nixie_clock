/*
 * Copyright © 2020 LambdAurora <aurora42lambda@gmail.com>
 *
 * This file is part of lambda_nixie_clock.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include <config.hpp>

Configuration::Configuration(EE24 eeprom) : eeprom(eeprom) {}

bool Configuration::has_valid_header() {
    // Header should be ASCII L N C (standing for LambdaNixieClock) then two digits representing the version.
    uint8_t header[5];
    if (!this->eeprom.read(0x00, header, 5))
        return false;
    // Version 01 (1.0.0)
    return header[0] == 'L' && header[1] == 'N' && header[2] == 'C' && header[3] == 0 && header[4] == 1;
}

bool Configuration::init() {
    bool available = false;
    for (size_t i = 0; i < 5; i++) { // 5 tries to connect to the EEPROM delayed by 500ms.
        available = eeprom.is_available();
        if (available)
            break;
        HAL_Delay(500);
    }
    if (!available)
        return false;

    if (!this->has_valid_header()) {
        // Let's set the default values!
        Config config = Configuration::get_default();
        this->write(config);
    }

    return true;
}

Config Configuration::get_default() {
    // Default values.
    Config config;
    config.h24 = true;
    config.cathode_poisoning_cycle = CPC_24H;
    return config;
}

bool Configuration::read(Config* config) {
    if (!this->eeprom.is_available())
        return false;

    if (!this->has_valid_header()) {
        *config = Configuration::get_default();
        return false;
    }

    uint8_t options[8];
    if (!this->eeprom.read(0x05, options, 8))
        return false;

    config->h24 = options[0] & 0x80;
    config->cathode_poisoning_cycle = static_cast<CathodePoisoningCycle>(options[1]);

    return true;
}

bool Configuration::write(const Config& config) {
    if (!this->eeprom.is_available())
        return false;

    if (!this->has_valid_header()) {
        // Header
        uint8_t header[5];
        header[0] = 'L';
        header[1] = 'N';
        header[2] = 'C';
        // Version 01 (1.0.0)
        header[3] = 0;
        header[4] = 1;
        if (!this->eeprom.write(0x00, header, 5))
            return false;
    }

    uint8_t options[8];
    for (uint8_t& option : options) option = 0;

    if (config.h24)
        options[0] |= 0x80;
    options[1] = config.cathode_poisoning_cycle;

    return this->eeprom.write(0x05, options, 8);
}

void Configuration::reset() {
    Config config = Configuration::get_default();
    this->write(config);
}
