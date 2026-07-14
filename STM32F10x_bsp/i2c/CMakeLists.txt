# I2C BSP for STM32F10x
add_library(aipi_bsp_i2c STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32f10x_bsp_i2c.c
)

target_include_directories(aipi_bsp_i2c
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
