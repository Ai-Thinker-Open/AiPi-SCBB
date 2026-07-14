# PWM+DMA BSP for STM32F10x
add_library(aipi_bsp_pwm_dma STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32f10x_pwm_dma.c
)

target_include_directories(aipi_bsp_pwm_dma
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
