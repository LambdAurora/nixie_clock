# Lambda Nixie Clock

An IN-14 (ИН-14) based nixie clock.

## Firmware

The firmware can be found in the `firmware` subdirectory.

## Main components

- [OtterPill](https://github.com/Jan--Henrik/OtterPill) - A STM32F072 devboard.
- [170V Nixie Power Supply](https://github.com/tonyp7/170v-nixie-power-supply)
- IN-14 (ИN-14) nixie tubes.
- K155ID1 - Binary decoders and nixie tube drivers.
- SN74HC595N - 8-bit shift registers.
- DS3231 - RTC module
- 24C32N - EEPROM
