/*
 * Copyright © 2020 LambdAurora <aurora42lambda@gmail.com>
 *
 * This file is part of lambda_nixie_clock.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef LAMBDA_NIXIE_CLOCK_CONSOLE_HPP
#define LAMBDA_NIXIE_CLOCK_CONSOLE_HPP

#include <common.h>
#include <string>
#include <cstdarg>

namespace console
{
    class outstream {
    private:
        std::string _buffer = "";

    public:
        outstream();

        void writef(const char* str, ...) {
            va_list ap;
            char buffer[512]; // Maybe you wanna increase the length of that buffer.
            va_start(ap, str);
            vsnprintf(buffer, 512, str, ap);
            va_end(ap);
            this->write(buffer);
        }

        void write(const char* str);

        void flush();

        outstream& operator<<(const std::string& str);

        outstream& operator<<(outstream& (*pf)(outstream&));

        outstream& operator<<(int i);

        outstream& operator<<(char ch);
    };

    outstream& operator<<(outstream& stream, const char* str);

    inline outstream& flush(outstream& stream) {
        stream.flush();
        return stream;
    }

    inline outstream& endl(outstream& stream) {
        return stream << "\r\n" << flush;
    }

    /**
     * Setups the UART console.
     */
    void setup();

    extern outstream cout;
}

#endif //LAMBDA_NIXIE_CLOCK_CONSOLE_HPP
