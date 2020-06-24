/*
 * Copyright © 2020 LambdAurora <aurora42lambda@gmail.com>
 *
 * This file is part of lambda_nixie_clock.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include <console.hpp>
#include <cstring>

static UART_HandleTypeDef console_uart1h = { nullptr };

namespace console
{
    outstream::outstream() = default;

    void outstream::write(const char* str) {
        this->flush();

        if (!str)
            return;
        size_t length = strlen(str);
        HAL_UART_Transmit(&console_uart1h, (uint8_t*) str, length, 300);
    }

    void outstream::flush() {
        if (this->_buffer.length() > 0) {
            std::string buffer = this->_buffer;
            this->_buffer = "";
            this->write(buffer.c_str());
        }
    }

    outstream& outstream::operator<<(const std::string& str) {
        this->_buffer += str;
        return *this;
    }

    outstream& outstream::operator<<(outstream& (* pf)(outstream&)) {
        return pf(*this);
    }

    outstream& outstream::operator<<(int i) {
        return this->operator<<(std::to_string(i));
    }

    outstream& outstream::operator<<(size_t i) {
        return this->operator<<(std::to_string(i));
    }

    outstream& outstream::operator<<(char ch) {
        this->_buffer += ch;
        return *this;
    }

    outstream& operator<<(outstream& stream, const char* str) {
        return stream << std::string(str);
    }

    void setup() {
        console_uart1h.Instance = USART1;
        console_uart1h.Init.BaudRate = 9600; // 115200
        console_uart1h.Init.WordLength = UART_WORDLENGTH_8B;
        console_uart1h.Init.StopBits = UART_STOPBITS_1;
        console_uart1h.Init.Parity = UART_PARITY_NONE;
        console_uart1h.Init.HwFlowCtl = UART_HWCONTROL_NONE;
        console_uart1h.Init.Mode = UART_MODE_TX_RX;

        HAL_UART_Init(&console_uart1h);
    }

    outstream cout = {};
}
