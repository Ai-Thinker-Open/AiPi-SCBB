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

# ─── Built-in BSP platform ─────────────────────────────────────────
set(SCBB_BSP_PLATFORM "Ai-M6x" CACHE STRING "Built-in BSP platform (Ai-M6x | stm32f10x)")

# ─── Read module enables from scbb_config.h ────────────────────────
# Parse SCBB_<MODULE>_ENABLED=1 defines from the generated config header
# so that menuconfig drives the build — no manual -D flags needed.
macro(scbb_read_config)
    set(_config_path "${CMAKE_CURRENT_SOURCE_DIR}/${SCBB_CONFIG_HEADER_PATH}")
    if(EXISTS "${_config_path}")
        file(READ "${_config_path}" _config_content)

        set(_all_modules CH224A SHT3X WS2812 HXD039B2 ST7789V_LCD RELAY DHT11 RD03_V2 OLED_096_SPI DS1302 INA226)
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

        # Read BSP platform (falls back to the cache value above)
        if(_config_content MATCHES "#define SCBB_BSP_PLATFORM \"([^\"]+)\"")
            set(SCBB_BSP_PLATFORM "${CMAKE_MATCH_1}")
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

# ST7789V_LCD (Bouffalo BL616/BL618 — 默认留空，直接使用 LHAL)
set(SCBB_ST7789V_LCD_SPI_HEADER    "bl616_bsp_spi.h"   CACHE STRING "ST7789V_LCD SPI BSP header")
set(SCBB_ST7789V_LCD_SPI_PREFIX    "bsp_spi"           CACHE STRING "ST7789V_LCD SPI BSP function prefix")
set(SCBB_ST7789V_LCD_GPIO_HEADER   "bl616_bsp_gpio.h"  CACHE STRING "ST7789V_LCD GPIO BSP header")
set(SCBB_ST7789V_LCD_GPIO_PREFIX   "bsp_gpio"          CACHE STRING "ST7789V_LCD GPIO BSP function prefix")
set(SCBB_ST7789V_LCD_DELAY_HEADER  "bl616_bsp_delay.h" CACHE STRING "ST7789V_LCD Delay BSP header")
set(SCBB_ST7789V_LCD_DELAY_PREFIX  "bsp_delay"         CACHE STRING "ST7789V_LCD Delay BSP function prefix")

# RELAY
set(SCBB_RELAY_GPIO_HEADER  "bl616_bsp_gpio.h"  CACHE STRING "RELAY GPIO BSP header")
set(SCBB_RELAY_GPIO_PREFIX  "bsp_gpio"          CACHE STRING "RELAY GPIO BSP function prefix")
set(SCBB_RELAY_DELAY_HEADER "bl616_bsp_delay.h" CACHE STRING "RELAY Delay BSP header")
set(SCBB_RELAY_DELAY_PREFIX "bsp_delay"         CACHE STRING "RELAY Delay BSP function prefix")

# DHT11
set(SCBB_DHT11_GPIO_HEADER  "bl616_bsp_gpio.h"  CACHE STRING "DHT11 GPIO BSP header")
set(SCBB_DHT11_GPIO_PREFIX  "bsp_gpio"          CACHE STRING "DHT11 GPIO BSP function prefix")
set(SCBB_DHT11_DELAY_HEADER "bl616_bsp_delay.h" CACHE STRING "DHT11 Delay BSP header")
set(SCBB_DHT11_DELAY_PREFIX "bsp_delay"         CACHE STRING "DHT11 Delay BSP function prefix")

# DS1302
set(SCBB_DS1302_GPIO_HEADER "bl616_bsp_gpio.h"  CACHE STRING "DS1302 GPIO BSP header")
set(SCBB_DS1302_GPIO_PREFIX "bsp_gpio"          CACHE STRING "DS1302 GPIO BSP function prefix")
set(SCBB_DS1302_DELAY_HEADER "bl616_bsp_delay.h" CACHE STRING "DS1302 Delay BSP header")
set(SCBB_DS1302_DELAY_PREFIX "bsp_delay"         CACHE STRING "DS1302 Delay BSP function prefix")

# INA226
set(SCBB_INA226_I2C_HEADER "bl616_bsp_i2c.h"   CACHE STRING "INA226 I2C BSP header")
set(SCBB_INA226_I2C_PREFIX "bsp_i2c"           CACHE STRING "INA226 I2C BSP function prefix")
set(SCBB_INA226_DELAY_HEADER "bl616_bsp_delay.h" CACHE STRING "INA226 Delay BSP header")
set(SCBB_INA226_DELAY_PREFIX "bsp_delay"         CACHE STRING "INA226 Delay BSP function prefix")

# OLED_096_SPI
set(SCBB_OLED_096_SPI_SPI_HEADER  "bl616_bsp_spi.h"   CACHE STRING "OLED_096_SPI SPI BSP header")
set(SCBB_OLED_096_SPI_SPI_PREFIX  "bsp_spi"           CACHE STRING "OLED_096_SPI SPI BSP function prefix")
set(SCBB_OLED_096_SPI_GPIO_HEADER "bl616_bsp_gpio.h"  CACHE STRING "OLED_096_SPI GPIO BSP header")
set(SCBB_OLED_096_SPI_GPIO_PREFIX "bsp_gpio"          CACHE STRING "OLED_096_SPI GPIO BSP function prefix")
set(SCBB_OLED_096_SPI_DELAY_HEADER "bl616_bsp_delay.h" CACHE STRING "OLED_096_SPI Delay BSP header")
set(SCBB_OLED_096_SPI_DELAY_PREFIX "bsp_delay"         CACHE STRING "OLED_096_SPI Delay BSP function prefix")

# RD03_V2
set(SCBB_RD03_V2_UART_HEADER "bl616_bsp_uart.h"   CACHE STRING "RD03_V2 UART BSP header")
set(SCBB_RD03_V2_UART_PREFIX "bsp_uart"           CACHE STRING "RD03_V2 UART BSP function prefix")
set(SCBB_RD03_V2_DELAY_HEADER "bl616_bsp_delay.h" CACHE STRING "RD03_V2 Delay BSP header")
set(SCBB_RD03_V2_DELAY_PREFIX "bsp_delay"         CACHE STRING "RD03_V2 Delay BSP function prefix")

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

# ST7789V_LCD module (Bouffalo BL616/BL618)
if(SCBB_ST7789V_LCD_ENABLED)
    list(APPEND SCBB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/ST7789V_LCD/axk_st7789v_lcd.c)
    list(APPEND SCBB_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/ST7789V_LCD/axk_st7789v_lcd.h)
    if(SCBB_IS_TOP_LEVEL)
        message(STATUS "AiPi-SCBB: ST7789V_LCD enabled (SPI+GPIO, BL616/BL618)")
    endif()
endif()

# RELAY module
if(SCBB_RELAY_ENABLED)
    list(APPEND SCBB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/RELAY/axk_relay.c)
    list(APPEND SCBB_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/RELAY/axk_relay.h)
    if(SCBB_IS_TOP_LEVEL)
        message(STATUS "AiPi-SCBB: RELAY enabled (GPIO)")
    endif()

# DHT11 module
if(SCBB_DHT11_ENABLED)
    list(APPEND SCBB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/DHT11/axk_dht11.c)
    list(APPEND SCBB_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/DHT11/axk_dht11.h)
    if(SCBB_IS_TOP_LEVEL)
        message(STATUS "AiPi-SCBB: DHT11 enabled (GPIO)")
    endif()

# DS1302 module
if(SCBB_DS1302_ENABLED)
    list(APPEND SCBB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/DS1302/axk_ds1302.c)
    list(APPEND SCBB_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/DS1302/axk_ds1302.h)
    if(SCBB_IS_TOP_LEVEL)
        message(STATUS "AiPi-SCBB: DS1302 enabled (GPIO)")
    endif()

# INA226 module
if(SCBB_INA226_ENABLED)
    list(APPEND SCBB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/INA226/axk_ina226.c)
    list(APPEND SCBB_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/INA226/axk_ina226.h)
    if(SCBB_IS_TOP_LEVEL)
        message(STATUS "AiPi-SCBB: INA226 enabled (I2C)")
    endif()
endif()
endif()

# OLED_096_SPI module
if(SCBB_OLED_096_SPI_ENABLED)
    list(APPEND SCBB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/OLED_096_SPI/axk_oled_096_spi.c)
    list(APPEND SCBB_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/OLED_096_SPI/axk_oled_096_spi.h)
    if(SCBB_IS_TOP_LEVEL)
        message(STATUS "AiPi-SCBB: OLED_096_SPI enabled (SPI+GPIO)")
    endif()
endif()

# RD03_V2 module
if(SCBB_RD03_V2_ENABLED)
    list(APPEND SCBB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/RD03_V2/axk_rd03_v2.c)
    list(APPEND SCBB_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/RD03_V2/axk_rd03_v2.h)
    if(SCBB_IS_TOP_LEVEL)
        message(STATUS "AiPi-SCBB: RD03_V2 enabled (UART)")
    endif()
endif()
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
    if(SCBB_ST7789V_LCD_ENABLED OR SCBB_OLED_096_SPI_ENABLED)
        list(APPEND SCBB_BSP_DIRS spi gpio delay)
    endif()
    if(SCBB_INA226_ENABLED)
        list(APPEND SCBB_BSP_DIRS i2c delay)
    endif()
    if(SCBB_RD03_V2_ENABLED)
        list(APPEND SCBB_BSP_DIRS uart delay)
    endif()
    if(SCBB_RELAY_ENABLED OR SCBB_DHT11_ENABLED OR SCBB_DS1302_ENABLED)
        list(APPEND SCBB_BSP_DIRS gpio delay)
    endif()

    # Multiple modules may share the same BSP protocol directory
    # (e.g. two I2C sensors both need i2c + delay). Deduplicate so
    # add_subdirectory() is only called once per directory.
    list(REMOVE_DUPLICATES SCBB_BSP_DIRS)

    foreach(dir ${SCBB_BSP_DIRS})
        if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/BSP/${SCBB_BSP_PLATFORM}/${dir})
            add_subdirectory(BSP/${SCBB_BSP_PLATFORM}/${dir})
        elseif(SCBB_IS_TOP_LEVEL)
            message(STATUS "AiPi-SCBB: BSP dir '${dir}' not found for platform '${SCBB_BSP_PLATFORM}' (skipped)")
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
    if(SCBB_ST7789V_LCD_ENABLED)
        target_compile_definitions(aipi_scbb PUBLIC SCBB_ST7789V_LCD_ENABLED=1)
    endif()
    if(SCBB_RELAY_ENABLED)
        target_compile_definitions(aipi_scbb PUBLIC SCBB_RELAY_ENABLED=1)
    endif()
    if(SCBB_DHT11_ENABLED)
        target_compile_definitions(aipi_scbb PUBLIC SCBB_DHT11_ENABLED=1)
    endif()
    if(SCBB_DS1302_ENABLED)
        target_compile_definitions(aipi_scbb PUBLIC SCBB_DS1302_ENABLED=1)
    endif()
    if(SCBB_INA226_ENABLED)
        target_compile_definitions(aipi_scbb PUBLIC SCBB_INA226_ENABLED=1)
    endif()
    if(SCBB_OLED_096_SPI_ENABLED)
        target_compile_definitions(aipi_scbb PUBLIC SCBB_OLED_096_SPI_ENABLED=1)
    endif()
    if(SCBB_RD03_V2_ENABLED)
        target_compile_definitions(aipi_scbb PUBLIC SCBB_RD03_V2_ENABLED=1)
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
        if(SCBB_ST7789V_LCD_ENABLED OR SCBB_OLED_096_SPI_ENABLED)
            if(TARGET aipi_bsp_spi)
                target_link_libraries(aipi_scbb PUBLIC aipi_bsp_spi)
            endif()
            if(TARGET aipi_bsp_gpio)
                target_link_libraries(aipi_scbb PUBLIC aipi_bsp_gpio)
            endif()
            if(TARGET aipi_bsp_delay)
                target_link_libraries(aipi_scbb PUBLIC aipi_bsp_delay)
            endif()
        endif()
        if(SCBB_INA226_ENABLED)
            if(TARGET aipi_bsp_i2c)
                target_link_libraries(aipi_scbb PUBLIC aipi_bsp_i2c)
            endif()
            if(TARGET aipi_bsp_delay)
                target_link_libraries(aipi_scbb PUBLIC aipi_bsp_delay)
            endif()
        endif()
        if(SCBB_RD03_V2_ENABLED)
            if(TARGET aipi_bsp_uart)
                target_link_libraries(aipi_scbb PUBLIC aipi_bsp_uart)
            endif()
            if(TARGET aipi_bsp_delay)
                target_link_libraries(aipi_scbb PUBLIC aipi_bsp_delay)
            endif()
        endif()
        if(SCBB_RELAY_ENABLED OR SCBB_DHT11_ENABLED OR SCBB_DS1302_ENABLED)
            if(TARGET aipi_bsp_gpio)
                target_link_libraries(aipi_scbb PUBLIC aipi_bsp_gpio)
            endif()
            if(TARGET aipi_bsp_delay)
                target_link_libraries(aipi_scbb PUBLIC aipi_bsp_delay)
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
        if(SCBB_ST7789V_LCD_ENABLED)
            target_compile_definitions(aipi_scbb PUBLIC
                "SCBB_ST7789V_LCD_SPI_HEADER=\"${SCBB_ST7789V_LCD_SPI_HEADER}\""
                "SCBB_ST7789V_LCD_SPI_PREFIX=${SCBB_ST7789V_LCD_SPI_PREFIX}"
                "SCBB_ST7789V_LCD_GPIO_HEADER=\"${SCBB_ST7789V_LCD_GPIO_HEADER}\""
                "SCBB_ST7789V_LCD_GPIO_PREFIX=${SCBB_ST7789V_LCD_GPIO_PREFIX}"
                "SCBB_ST7789V_LCD_DELAY_HEADER=\"${SCBB_ST7789V_LCD_DELAY_HEADER}\""
                "SCBB_ST7789V_LCD_DELAY_PREFIX=${SCBB_ST7789V_LCD_DELAY_PREFIX}"
            )
        endif()
    endif()
endif()

if(SCBB_IS_TOP_LEVEL)
    message(STATUS "AiPi-SCBB: Library target 'aipi_scbb' created")
endif()
