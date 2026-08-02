# Delay BSP for STM32F10x
add_library(aipi_bsp_delay STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32f10x_delay.c
)

target_include_directories(aipi_bsp_delay
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
