cmake_minimum_required(VERSION 3.2.0)

set(CMAKE_TOOLCHAIN_FILE "toolchain/gcc.cmake" CACHE PATH "toolchain file")

project(bootloader LANGUAGES C ASM)

function(add_bootloader device)
    string(TOUPPER ${device} device_upper)

    add_executable(${device}.elf
        src/startup.c

        src/boot.c
        src/hid.c
        src/main.c
        src/usb.c

        deps/system_stm32f1xx.c
        deps/startup_stm32f103x6.s
        deps/stm32f1xx_ll_utils.c
    )

    target_compile_options(${device}.elf PUBLIC
        -mthumb
        -mcpu=cortex-m3
        -fno-strict-aliasing

        -g
        -Os
        -flto

        -Wall
        -Wextra
        -Wcast-align
        -Wpointer-arith
        -Wredundant-decls
        -Wshadow
    )

    target_include_directories(${device}.elf PUBLIC
        src
        deps
    )

    target_compile_definitions(${device}.elf PUBLIC
        TARGET_${device_upper}
        STM32F103x6
    )

    target_link_libraries(${device}.elf PUBLIC
        -nostartfiles
        -nostdlib
        "-T ${PROJECT_SOURCE_DIR}/src/STM32F103C8T6.ld"
        -lgcc
    )

    add_custom_target(bootloader-${device}.bin ALL
        COMMAND arm-none-eabi-objcopy -j .isr_vector -j .text -j .rodata -j .data -O binary $<TARGET_FILE:${device}.elf> ${CMAKE_BINARY_DIR}/bootloader-${device}.bin
        DEPENDS ${device}.elf
        BYPRODUCTS bootloader-${device}.bin
    )
endfunction(add_bootloader)

add_bootloader(generic)
add_bootloader(vial_test)
