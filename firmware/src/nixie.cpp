/*
 * Copyright © 2020 LambdAurora <aurora42lambda@gmail.com>
 *
 * This file is part of lambda_nixie_clock.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include <nixie.hpp>

bool NixieState::show_number() const {
    return this->_show_number;
}

void NixieState::show_number(bool show) {
    this->_show_number = show;
    if (!show)
        this->_number = 0;
}

void NixieState::on() {
    this->_show_number = this->_left_comma = this->_right_comma = true;
}

void NixieState::off() {
    this->_show_number = this->_left_comma = this->_right_comma = false;
}

uint8_t NixieState::number() const {
    return this->_number;
}

void NixieState::number(uint8_t number) {
    if (number > 9) {
        this->show_number(false);
        return;
    }
    this->_number = number;
    this->_show_number = true;
}

bool NixieState::left_comma() const {
    return this->_left_comma;
}

void NixieState::left_comma(bool left_comma) {
    this->_left_comma = left_comma;
}

bool NixieState::right_comma() const {
    return this->_right_comma;
}

void NixieState::right_comma(bool right_comma) {
    this->_right_comma = right_comma;
}

void NixieState::clear() {
    this->off();
    this->_number = 0;
}

void NixieState::from_string(const std::string& str) {
    bool show_number = false;
    for (char c : str) {
        if (std::isdigit(c)) {
            show_number = true;
            this->number(c - 0x30); // ASCII 0 is 0x30.
            break;
        }
    }
    this->show_number(show_number);
    this->left_comma(str.find('.') != std::string::npos);
    this->right_comma(str.find(',') != std::string::npos);
}

std::string NixieState::to_string() const {
    std::string str = this->_left_comma ? "." : "";
    if (this->_show_number)
        str.append(static_cast<size_t>(1), this->_number + 0x30); // ASCII 0 is 0x30.
    if (this->_right_comma)
        str.append(",");
    return str;
}

void NixieState::to_binary(nixie_state_array& array) {
    if (!this->_show_number) {
        for (size_t i = 0; i < 4; i++)
            array[i] = true; // 0b1111 to the K155ID1 binary decoder as it shutdowns the number on the nixie tube.
    } else {
        for (size_t i = 0; i < 4; i++)
            array[i] = (this->_number & (0x08 >> i)) > 0;
    }
    array[4] = this->_left_comma;
    array[5] = this->_right_comma;
}

void NixieState::push(bool commas) {
    nixie_state_array states;
    this->to_binary(states);
    for (size_t i = (commas ? 6 : 4); i > 0; i--) {
        HAL_GPIO_WritePin(SDI_GPIO_PORT, SDI_PIN, states[i - 1] ? GPIO_PIN_SET : GPIO_PIN_RESET);
        shift_register_pulse(SRCLK_GPIO_PORT, SRCLK_PIN);
    }
}

NixieArray::NixieArray() {
    for (size_t i = 0; i < NIXIE_COUNT; i++) {
        this->array[i].show_number(false);
        this->array[i].left_comma(false);
        this->array[i].right_comma(false);
    }
}

void NixieArray::from_string(const std::string& str) {
    size_t nixie = 0, last_pos = 0, pos = 0;
    std::string token;
    while ((pos = str.find(';', last_pos)) != std::string::npos && nixie < NIXIE_COUNT) {
        token = str.substr(last_pos, pos - last_pos);
        last_pos = pos + 1;
        this->array[nixie].from_string(token);
        nixie++;
    }
    if (nixie < NIXIE_COUNT)
        this->array[nixie].from_string(str.substr(last_pos));
}

NixieState& NixieArray::at(size_t i) {
    return this->array[i];
}

void NixieArray::display(bool commas) {
    for (size_t i = NIXIE_COUNT; i > 0; i--) {
        this->array[i - 1].push(commas);
    }
    shift_register_pulse(RCLK_GPIO_PORT, RCLK_PIN);
}

std::string NixieArray::to_string() const {
    std::string str;
    for (size_t i = 0; i < NIXIE_COUNT; i++) {
        str.append(this->array[i].to_string());
        if (i < NIXIE_COUNT - 1)
            str.append(";");
    }
    return str;
}
