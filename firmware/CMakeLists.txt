cmake_minimum_required(VERSION 3.12)
project(lambda_nixie_clock C CXX ASM)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(STM32CubeF0_ROOT "${CMAKE_SOURCE_DIR}/lib/STM32CubeF0")

include(embedded/Otterpill)
include(utils/CompileUtility)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Generate compile flags.
generate_flags(COMPILE_FLAGS "native" 2 true)
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -Wall -fdata-sections -ffunction-sections")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}${COMPILE_FLAGS} -Wall -mcpu=cortex-m0 -lc -lm -lnosys -MMD -MP -Wall -fdata-sections -ffunction-sections")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}${COMPILE_FLAGS} -Wall -mcpu=cortex-m0 -Wall -fdata-sections -ffunction-sections")

find_package(STM32CubeF0 REQUIRED)

set_linker_script(${CMAKE_CURRENT_SOURCE_DIR}/STM32F072C8Tx_FLASH.ld)

include_directories(include ${STM32CubeF0_INCLUDE_DIRS})

add_otterpill_executable(lambda_nixie_clock ${STM32CubeF0_SOURCES}
        src/startup_stm32f072xb.s
        src/system_stm32f0xx.c
        src/stm32f0xx_hal_msp.c
        src/stm32f0xx_it.c
        src/common.c
        src/rtc.cpp
        src/ee24.cpp
        src/console.cpp
        src/main.cpp)
