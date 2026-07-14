cmake_minimum_required(VERSION 3.15)

# Only set project if this is the top-level CMakeLists.txt
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(AiPi-SCBB
        VERSION 0.1.0
        LANGUAGES C
        DESCRIPTION "AiPi-SCBB peripheral driver library"
    )
    set(SCBB_IS_TOP_LEVEL ON)
else()
    set(SCBB_IS_TOP_LEVEL OFF)
endif()

# Module options (cache variables, can be overridden by parent project)
option(SCBB_CH224A    "Enable CH224A USB-PD driver (I2C)"          OFF)
option(SCBB_SHT3X     "Enable SHT3x temperature/humidity sensor (I2C)" OFF)
option(SCBB_WS2812    "Enable WS2812 LED strip driver (PWM+DMA)"  OFF)
option(SCBB_HXD039B2  "Enable HXD039B2 IR encoder/decoder (UART)" OFF)

# BSP options (set ON if using built-in BSP)
option(SCBB_USE_BSP   "Use built-in STM32F10x BSP"                OFF)

# Custom BSP header names (for SCBB_USE_BSP=OFF)
set(SCBB_BSP_I2C_HEADER "stm32f10x_bsp_i2c.h" CACHE STRING "Custom I2C BSP header")
set(SCBB_BSP_UART_HEADER "stm32f10x_bsp_uart.h" CACHE STRING "Custom UART BSP header")
set(SCBB_BSP_SPI_HEADER "stm32f10x_bsp_spi.h" CACHE STRING "Custom SPI BSP header")
set(SCBB_BSP_GPIO_HEADER "stm32f10x_bsp_gpio.h" CACHE STRING "Custom GPIO BSP header")
set(SCBB_BSP_DELAY_HEADER "stm32f10x_delay.h" CACHE STRING "Custom delay BSP header")

# Collect enabled modules and their BSP requirements
set(SCBB_SOURCES "")
set(SCBB_HEADERS "")
set(SCBB_NEED_I2C    OFF)
set(SCBB_NEED_PWM_DMA OFF)
set(SCBB_NEED_UART   OFF)
set(SCBB_NEED_GPIO   OFF)
set(SCBB_NEED_DELAY  OFF)

# CH224A module
if(SCBB_CH224A)
    list(APPEND SCBB_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/CH224A/axk_ch224.c
    )
    list(APPEND SCBB_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/CH224A/axk_ch224.h
    )
    set(SCBB_NEED_I2C ON)
    set(SCBB_NEED_DELAY ON)
    if(SCBB_IS_TOP_LEVEL)
        message(STATUS "AiPi-SCBB: CH224A enabled (I2C)")
    endif()
endif()

# SHT3x module
if(SCBB_SHT3X)
    list(APPEND SCBB_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/SHT3x/axk_sht3x.c
    )
    list(APPEND SCBB_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/SHT3x/axk_sht3x.h
    )
    set(SCBB_NEED_I2C ON)
    set(SCBB_NEED_DELAY ON)
    if(SCBB_IS_TOP_LEVEL)
        message(STATUS "AiPi-SCBB: SHT3x enabled (I2C)")
    endif()
endif()

# WS2812 module
if(SCBB_WS2812)
    list(APPEND SCBB_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/WS2812/axk_ws2812.c
        ${CMAKE_CURRENT_SOURCE_DIR}/WS2812/color_mode.c
    )
    list(APPEND SCBB_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/WS2812/axk_ws2812.h
        ${CMAKE_CURRENT_SOURCE_DIR}/WS2812/color_mode.h
    )
    set(SCBB_NEED_PWM_DMA ON)
    if(SCBB_IS_TOP_LEVEL)
        message(STATUS "AiPi-SCBB: WS2812 enabled (PWM+DMA)")
    endif()
endif()

# HXD039B2 module
if(SCBB_HXD039B2)
    list(APPEND SCBB_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/HXD039B2/axk_hxd039b2.c
    )
    list(APPEND SCBB_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/HXD039B2/axk_hxd039b2.h
    )
    set(SCBB_NEED_UART ON)
    set(SCBB_NEED_GPIO ON)
    set(SCBB_NEED_DELAY ON)
    if(SCBB_IS_TOP_LEVEL)
        message(STATUS "AiPi-SCBB: HXD039B2 enabled (UART+GPIO)")
    endif()
endif()

# Include built-in BSP if enabled
if(SCBB_USE_BSP)
    if(SCBB_NEED_I2C)
        add_subdirectory(STM32F10x_bsp/i2c)
    endif()

    if(SCBB_NEED_PWM_DMA)
        add_subdirectory(STM32F10x_bsp/pwm_dma)
    endif()

    if(SCBB_NEED_DELAY)
        add_subdirectory(STM32F10x_bsp/delay)
    endif()

    if(SCBB_NEED_UART)
        # TODO: Add UART BSP when available
        if(SCBB_IS_TOP_LEVEL)
            message(STATUS "AiPi-SCBB: UART BSP required (not yet implemented)")
        endif()
    endif()

    if(SCBB_NEED_GPIO)
        # TODO: Add GPIO BSP when available
        if(SCBB_IS_TOP_LEVEL)
            message(STATUS "AiPi-SCBB: GPIO BSP required (not yet implemented)")
        endif()
    endif()
else()
    if(SCBB_IS_TOP_LEVEL)
        message(STATUS "AiPi-SCBB: Built-in BSP disabled, using custom BSP")
    endif()
endif()

# Create library target
add_library(aipi_scbb STATIC)

# ALIAS target for use in other CMake projects
add_library(AiPi::SCBB ALIAS aipi_scbb)

if(SCBB_SOURCES)
    target_sources(aipi_scbb PRIVATE ${SCBB_SOURCES})

    target_include_directories(aipi_scbb
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<INSTALL_INTERFACE:include>
    )

    # Link BSP libraries (only if using built-in BSP)
    if(SCBB_USE_BSP)
        if(SCBB_NEED_I2C)
            target_link_libraries(aipi_scbb PUBLIC aipi_bsp_i2c)
        endif()

        if(SCBB_NEED_PWM_DMA)
            target_link_libraries(aipi_scbb PUBLIC aipi_bsp_pwm_dma)
        endif()

        if(SCBB_NEED_DELAY)
            target_link_libraries(aipi_scbb PUBLIC aipi_bsp_delay)
        endif()
    else()
        # Export custom BSP header names for user-provided BSP
        target_compile_definitions(aipi_scbb PRIVATE
            SCBB_BSP_I2C_HEADER="${SCBB_BSP_I2C_HEADER}"
            SCBB_BSP_UART_HEADER="${SCBB_BSP_UART_HEADER}"
            SCBB_BSP_SPI_HEADER="${SCBB_BSP_SPI_HEADER}"
            SCBB_BSP_GPIO_HEADER="${SCBB_BSP_GPIO_HEADER}"
            SCBB_BSP_DELAY_HEADER="${SCBB_BSP_DELAY_HEADER}"
        )
    endif()

    # Export compile definitions for enabled modules
    if(SCBB_CH224A)
        target_compile_definitions(aipi_scbb PUBLIC SCBB_CH224A_ENABLED=1)
    endif()
    if(SCBB_SHT3X)
        target_compile_definitions(aipi_scbb PUBLIC SCBB_SHT3X_ENABLED=1)
    endif()
    if(SCBB_WS2812)
        target_compile_definitions(aipi_scbb PUBLIC SCBB_WS2812_ENABLED=1)
    endif()
    if(SCBB_HXD039B2)
        target_compile_definitions(aipi_scbb PUBLIC SCBB_HXD039B2_ENABLED=1)
    endif()
endif()

if(SCBB_IS_TOP_LEVEL)
    message(STATUS "AiPi-SCBB: Library target 'aipi_scbb' created")
endif()
