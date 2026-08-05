add_library(aipi_bsp_spi STATIC ${CMAKE_CURRENT_SOURCE_DIR}/stm32f10x_bsp_spi.c)
target_include_directories(aipi_bsp_spi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
