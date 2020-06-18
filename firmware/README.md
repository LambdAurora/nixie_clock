# Lambda Nixie Clock firmware

Firmware for the Lambda Nixie Clock.

## Requirements

 - Git submodules initialized.
 - An arm-none-eabi GCC toolchain.
 - CMake
 - Make

## Build

Go into the `build` directory and execute `cmake -DCMAKE_TOOLCHAIN_FILE="../cmake/toolchains/arm_none_eabi_gcc.cmake" ..` then `make`.

To flash it into the OtterPill (or any other STM32F0 based hardware), enter DFU mode and execute `make lambda_nixie_clock.dfu`.
