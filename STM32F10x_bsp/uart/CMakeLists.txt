add_library(aipi_bsp_uart STATIC ${CMAKE_CURRENT_SOURCE_DIR}/stm32f10x_bsp_uart.c)
target_include_directories(aipi_bsp_uart PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
