/*
 * Copyright © 2020 LambdAurora <aurora42lambda@gmail.com>
 *
 * This file is part of lambda_nixie_clock.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef LAMBDA_NIXIE_CLOCK_NIXIE_HPP
#define LAMBDA_NIXIE_CLOCK_NIXIE_HPP

#include <common.h>
#include <array>
#include <functional>

typedef std::array<bool, 6> nixie_state_array;

/**
 * Represents a Nixie tube state.
 */
class NixieState
{
private:
    bool _show_number = false;
    /** Represents the 4-bit number displayed by the nixie tube. */
    uint8_t _number = 0;
    bool _left_comma = false;
    bool _right_comma = false;

public:
    /**
     * Returns whether the nixie tube should show a number.
     * @return True if the nixie tube should show a number, else false.
     */
    [[nodiscard]] bool show_number() const;

    /**
     * Sets whether the nixie tube should show a number.
     * @param show True if the nixie tube should show a number.
     */
    void show_number(bool show);

    /**
     * Turns on the nixie tube.
     */
    void on();

    /**
     * Turns off the nixie tube.
     */
    void off();

    /**
     * Returns the number the nixie tube displays.
     * @return The number the nixie tube displays.
     */
    [[nodiscard]] uint8_t number() const;

    /**
     * Sets the number the nixie tube displays.
     * @param number A number between 0 and 9 (included).
     */
    void number(uint8_t number);

    /**
     * Returns whether the left comma is displayed.
     * @return True if the left comma is displayed, else false.
     */
    [[nodiscard]] bool left_comma() const;

    /**
     * Sets whether the left comma is displayed.
     * @param left_comma True if the left comma is displayed, else false.
     */
    void left_comma(bool left_comma);

    /**
     * Returns whether the right comma is displayed.
     * @return True if the right comma is displayed.
     */
    [[nodiscard]] bool right_comma() const;

    /**
     * Sets whether the right comma is displayed.
     * @param right_comma True if the right comma is displayed.
     */
    void right_comma(bool right_comma);

    /**
     * Clears the nixie tube.
     *
     * The difference with {@link off()} is this also resets the number.
     */
    void clear();

    void from_string(const std::string& str);

    [[nodiscard]] std::string to_string() const;

    /**
     * Returns the nixie tube state as a binary array.
     * @param array The array.
     */
    void to_binary(nixie_state_array& array);

    /**
     * Push to the shift registers the nixie tube state.
     * @param commas True if commas are supported, else false.
     */
    void push(bool commas = true);
};

/**
 * Represents the nixie tube array of size NIXIE_COUNT.
 */
class NixieArray
{
private:
    std::array<NixieState, NIXIE_COUNT> array;

public:
    NixieArray();

    void from_string(const std::string& str);

    NixieState& at(size_t i);

    /**
     * Executes a provided function once for each array element.
     * @param callback Function to execute on each element.
     * @return This array.
     */
    template<typename _Funct>
    NixieArray& for_each(_Funct callback) {
        for (size_t index = 0; index < NIXIE_COUNT; index++) {
            callback(index, this->array[index]);
        }
        return *this;
    }

    /**
     * Resets every nixie tube state.
     */
    void reset();

    [[nodiscard]] constexpr inline size_t size() const {
        return NIXIE_COUNT;
    }

    /**
     * Pushes to the shift register all the nixie tube states of this array.
     * @param commas True if commas are supported, else false.
     */
    void display(bool commas = true);

    [[nodiscard]] std::string to_string() const;
};

#endif //LAMBDA_NIXIE_CLOCK_NIXIE_HPP
