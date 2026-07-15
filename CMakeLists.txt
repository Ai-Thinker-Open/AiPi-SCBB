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

# ─── Config header path ────────────────────────────────────────────
set(SCBB_CONFIG_HEADER_PATH   "scbb_config.h"        CACHE STRING "Path to scbb_config.h")

# ─── Read module enables from scbb_config.h ────────────────────────
# Parse SCBB_<MODULE>_ENABLED=1 defines from the generated config header
# so that menuconfig drives the build — no manual -D flags needed.
macro(scbb_read_config)
    set(_config_path "${CMAKE_CURRENT_SOURCE_DIR}/${SCBB_CONFIG_HEADER_PATH}")
    if(EXISTS "${_config_path}")
        file(READ "${_config_path}" _config_content)

        set(_all_modules CH224A SHT3X WS2812 HXD039B2)
        foreach(_mod ${_all_modules})
            if(_config_content MATCHES "#define SCBB_${_mod}_ENABLED")
                set(SCBB_${_mod}_ENABLED ON)
            else()
                set(SCBB_${_mod}_ENABLED OFF)
            endif()
        endforeach()

        # Read BSP option
        if(_config_content MATCHES "#define SCBB_USE_BSP")
            set(SCBB_USE_BSP ON)
        else()
            set(SCBB_USE_BSP OFF)
        endif()
    else()
        if(SCBB_IS_TOP_LEVEL)
            message(STATUS "AiPi-SCBB: ${SCBB_CONFIG_HEADER_PATH} not found, all modules OFF")
        endif()
        set(SCBB_CH224A_ENABLED OFF)
        set(SCBB_SHT3X_ENABLED OFF)
        set(SCBB_WS2812_ENABLED OFF)
        set(SCBB_HXD039B2_ENABLED OFF)
        set(SCBB_USE_BSP OFF)
    endif()
endmacro()

scbb_read_config()

# ─── Per-module BSP header options ─────────────────────────────────
# CH224A
set(SCBB_CH224A_I2C_HEADER    "stm32f10x_bsp_i2c.h"  CACHE STRING "CH224A I2C BSP header")
set(SCBB_CH224A_I2C_PREFIX    "bsp_i2c"              CACHE STRING "CH224A I2C BSP function prefix")
set(SCBB_CH224A_DELAY_HEADER  "stm32f10x_delay.h"    CACHE STRING "CH224A Delay BSP header")
set(SCBB_CH224A_DELAY_PREFIX  "delay"                CACHE STRING "CH224A Delay BSP function prefix")

# SHT3x
set(SCBB_SHT3X_I2C_HEADER     "stm32f10x_bsp_i2c.h"  CACHE STRING "SHT3x I2C BSP header")
set(SCBB_SHT3X_I2C_PREFIX     "bsp_i2c"              CACHE STRING "SHT3x I2C BSP function prefix")
set(SCBB_SHT3X_DELAY_HEADER   "stm32f10x_delay.h"    CACHE STRING "SHT3x Delay BSP header")
set(SCBB_SHT3X_DELAY_PREFIX   "delay"                CACHE STRING "SHT3x Delay BSP function prefix")

# WS2812
set(SCBB_WS2812_PWM_DMA_HEADER  "stm32f10x_pwm_dma.h"  CACHE STRING "WS2812 PWM DMA BSP header")
set(SCBB_WS2812_PWM_DMA_PREFIX  "bsp_pwm_dma"          CACHE STRING "WS2812 PWM DMA BSP function prefix")

# HXD039B2
set(SCBB_HXD039B2_UART_HEADER   "stm32f10x_bsp_uart.h" CACHE STRING "HXD039B2 UART BSP header")
set(SCBB_HXD039B2_UART_PREFIX   "bsp_uart"             CACHE STRING "HXD039B2 UART BSP function prefix")
set(SCBB_HXD039B2_GPIO_HEADER   "stm32f10x_bsp_gpio.h" CACHE STRING "HXD039B2 GPIO BSP header")
set(SCBB_HXD039B2_GPIO_PREFIX   "bsp_gpio"             CACHE STRING "HXD039B2 GPIO BSP function prefix")
set(SCBB_HXD039B2_DELAY_HEADER  "stm32f10x_delay.h"    CACHE STRING "HXD039B2 Delay BSP header")
set(SCBB_HXD039B2_DELAY_PREFIX  "delay"                CACHE STRING "HXD039B2 Delay BSP function prefix")

# ─── Collect enabled modules ───────────────────────────────────────
set(SCBB_SOURCES "")
set(SCBB_HEADERS "")

# CH224A module
if(SCBB_CH224A_ENABLED)
    list(APPEND SCBB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/CH224A/axk_ch224.c)
    list(APPEND SCBB_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/CH224A/axk_ch224.h)
    if(SCBB_IS_TOP_LEVEL)
        message(STATUS "AiPi-SCBB: CH224A enabled (I2C)")
    endif()
endif()

# SHT3x module
if(SCBB_SHT3X_ENABLED)
    list(APPEND SCBB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/SHT3x/axk_sht3x.c)
    list(APPEND SCBB_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/SHT3x/axk_sht3x.h)
    if(SCBB_IS_TOP_LEVEL)
        message(STATUS "AiPi-SCBB: SHT3x enabled (I2C)")
    endif()
endif()

# WS2812 module
if(SCBB_WS2812_ENABLED)
    list(APPEND SCBB_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/WS2812/axk_ws2812.c
        ${CMAKE_CURRENT_SOURCE_DIR}/WS2812/color_mode.c
    )
    list(APPEND SCBB_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/WS2812/axk_ws2812.h
        ${CMAKE_CURRENT_SOURCE_DIR}/WS2812/color_mode.h
    )
    if(SCBB_IS_TOP_LEVEL)
        message(STATUS "AiPi-SCBB: WS2812 enabled (PWM+DMA)")
    endif()
endif()

# HXD039B2 module
if(SCBB_HXD039B2_ENABLED)
    list(APPEND SCBB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/HXD039B2/axk_hxd039b2.c)
    list(APPEND SCBB_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/HXD039B2/axk_hxd039b2.h)
    if(SCBB_IS_TOP_LEVEL)
        message(STATUS "AiPi-SCBB: HXD039B2 enabled (UART+GPIO)")
    endif()
endif()

# ─── Include built-in BSP if enabled ──────────────────────────────
if(SCBB_USE_BSP)
    # Collect BSP dependencies from enabled modules
    set(SCBB_BSP_DIRS "")
    if(SCBB_CH224A_ENABLED OR SCBB_SHT3X_ENABLED OR SCBB_HXD039B2_ENABLED)
        list(APPEND SCBB_BSP_DIRS i2c delay)
    endif()
    if(SCBB_WS2812_ENABLED)
        list(APPEND SCBB_BSP_DIRS pwm_dma)
    endif()
    if(SCBB_HXD039B2_ENABLED)
        list(APPEND SCBB_BSP_DIRS uart gpio)
    endif()

    foreach(dir ${SCBB_BSP_DIRS})
        if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/STM32F10x_bsp/${dir})
            add_subdirectory(STM32F10x_bsp/${dir})
        elseif(SCBB_IS_TOP_LEVEL)
            message(STATUS "AiPi-SCBB: BSP dir '${dir}' not found (skipped)")
        endif()
    endforeach()
endif()

# ─── Create library target ────────────────────────────────────────
if(SCBB_SOURCES)
    add_library(aipi_scbb STATIC)
else()
    add_library(aipi_scbb INTERFACE)
endif()

add_library(AiPi::SCBB ALIAS aipi_scbb)

if(SCBB_SOURCES)
    target_sources(aipi_scbb PRIVATE ${SCBB_SOURCES})

    # Get directory containing scbb_config.h for include path
    get_filename_component(SCBB_CONFIG_DIR "${SCBB_CONFIG_HEADER_PATH}" DIRECTORY)
    if(SCBB_CONFIG_DIR STREQUAL "")
        set(SCBB_CONFIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    else()
        set(SCBB_CONFIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${SCBB_CONFIG_DIR}")
    endif()

    target_include_directories(aipi_scbb
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<BUILD_INTERFACE:${SCBB_CONFIG_DIR}>
            $<INSTALL_INTERFACE:include>
    )

    # Export module enable defines
    if(SCBB_CH224A_ENABLED)
        target_compile_definitions(aipi_scbb PUBLIC SCBB_CH224A_ENABLED=1)
    endif()
    if(SCBB_SHT3X_ENABLED)
        target_compile_definitions(aipi_scbb PUBLIC SCBB_SHT3X_ENABLED=1)
    endif()
    if(SCBB_WS2812_ENABLED)
        target_compile_definitions(aipi_scbb PUBLIC SCBB_WS2812_ENABLED=1)
    endif()
    if(SCBB_HXD039B2_ENABLED)
        target_compile_definitions(aipi_scbb PUBLIC SCBB_HXD039B2_ENABLED=1)
    endif()

    # Link built-in BSP libraries
    if(SCBB_USE_BSP)
        if(SCBB_CH224A_ENABLED OR SCBB_SHT3X_ENABLED OR SCBB_HXD039B2_ENABLED)
            if(TARGET aipi_bsp_i2c)
                target_link_libraries(aipi_scbb PUBLIC aipi_bsp_i2c)
            endif()
            if(TARGET aipi_bsp_delay)
                target_link_libraries(aipi_scbb PUBLIC aipi_bsp_delay)
            endif()
        endif()
        if(SCBB_WS2812_ENABLED AND TARGET aipi_bsp_pwm_dma)
            target_link_libraries(aipi_scbb PUBLIC aipi_bsp_pwm_dma)
        endif()
        if(SCBB_HXD039B2_ENABLED)
            if(TARGET aipi_bsp_uart)
                target_link_libraries(aipi_scbb PUBLIC aipi_bsp_uart)
            endif()
            if(TARGET aipi_bsp_gpio)
                target_link_libraries(aipi_scbb PUBLIC aipi_bsp_gpio)
            endif()
        endif()
    else()
        # Export per-module BSP headers and prefixes as compile definitions
        if(SCBB_CH224A_ENABLED)
            target_compile_definitions(aipi_scbb PUBLIC
                "SCBB_CH224A_I2C_HEADER=\"${SCBB_CH224A_I2C_HEADER}\""
                "SCBB_CH224A_I2C_PREFIX=${SCBB_CH224A_I2C_PREFIX}"
                "SCBB_CH224A_DELAY_HEADER=\"${SCBB_CH224A_DELAY_HEADER}\""
                "SCBB_CH224A_DELAY_PREFIX=${SCBB_CH224A_DELAY_PREFIX}"
            )
        endif()
        if(SCBB_SHT3X_ENABLED)
            target_compile_definitions(aipi_scbb PUBLIC
                "SCBB_SHT3X_I2C_HEADER=\"${SCBB_SHT3X_I2C_HEADER}\""
                "SCBB_SHT3X_I2C_PREFIX=${SCBB_SHT3X_I2C_PREFIX}"
                "SCBB_SHT3X_DELAY_HEADER=\"${SCBB_SHT3X_DELAY_HEADER}\""
                "SCBB_SHT3X_DELAY_PREFIX=${SCBB_SHT3X_DELAY_PREFIX}"
            )
        endif()
        if(SCBB_WS2812_ENABLED)
            target_compile_definitions(aipi_scbb PUBLIC
                "SCBB_WS2812_PWM_DMA_HEADER=\"${SCBB_WS2812_PWM_DMA_HEADER}\""
                "SCBB_WS2812_PWM_DMA_PREFIX=${SCBB_WS2812_PWM_DMA_PREFIX}"
            )
        endif()
        if(SCBB_HXD039B2_ENABLED)
            target_compile_definitions(aipi_scbb PUBLIC
                "SCBB_HXD039B2_UART_HEADER=\"${SCBB_HXD039B2_UART_HEADER}\""
                "SCBB_HXD039B2_UART_PREFIX=${SCBB_HXD039B2_UART_PREFIX}"
                "SCBB_HXD039B2_GPIO_HEADER=\"${SCBB_HXD039B2_GPIO_HEADER}\""
                "SCBB_HXD039B2_GPIO_PREFIX=${SCBB_HXD039B2_GPIO_PREFIX}"
                "SCBB_HXD039B2_DELAY_HEADER=\"${SCBB_HXD039B2_DELAY_HEADER}\""
                "SCBB_HXD039B2_DELAY_PREFIX=${SCBB_HXD039B2_DELAY_PREFIX}"
            )
        endif()
    endif()
endif()

if(SCBB_IS_TOP_LEVEL)
    message(STATUS "AiPi-SCBB: Library target 'aipi_scbb' created")
endif()
