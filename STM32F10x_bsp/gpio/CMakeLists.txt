add_library(aipi_bsp_gpio STATIC ${CMAKE_CURRENT_SOURCE_DIR}/stm32f10x_bsp_gpio.c)
target_include_directories(aipi_bsp_gpio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
