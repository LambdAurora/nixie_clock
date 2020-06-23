/*
 * Copyright © 2020 LambdAurora <aurora42lambda@gmail.com>
 *
 * This file is part of lambda_nixie_clock.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef LAMBDA_NIXIE_CLOCK_CONFIG_HPP
#define LAMBDA_NIXIE_CLOCK_CONFIG_HPP

#include <ee24.hpp>

/**
 * Represents when the cathode poisoning prevention cycle fires.
 */
enum CathodePoisoningCycle : uint8_t {
    CPC_24H = 124,
    CPC_12H = 112,
    CPC_6H = 160,
    CPC_HOUR = 110,
    CPC_30M = 030,
    CPC_5M = 005
};

/**
 * Represents the configuration.
 */
typedef struct {
    bool h24; // 24H clock format.
    CathodePoisoningCycle cathode_poisoning_cycle;
} Config;

/**
 * Represents the configuration manager.
 */
class Configuration {
private:
    EE24 eeprom;

    /**
     * Returns whether the EEPROM has a valid configuration header.
     * @return True if the EEPROM has a valid header, else false.
     */
    bool has_valid_header();

public:
    explicit Configuration(EE24 eeprom);

    /**
     * Initializes the configuration.
     * @return True if successfully initialized, else false.
     */
    bool init();

    /**
     * Gets the default configuration.
     * @param config The default configuration.
     */
    [[nodiscard]] static Config get_default();

    /**
     * Reads the configuration from the EEPROM.
     * @param config The configuration.
     * @return True if the configuration was read successfully, else false.
     */
    bool read(Config* config);

    /**
     * Writes the configuration to the EEPROM.
     * @param config The configuration to write.
     * @return True if the configuration was written successfully, else false.
     */
    bool write(const Config& config);

    /**
     * Resets the configuration in the EEPROM.
     */
    void reset();
};

#endif //LAMBDA_NIXIE_CLOCK_CONFIG_HPP
