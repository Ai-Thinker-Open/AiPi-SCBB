add_library(aipi_bsp_i2c STATIC ${CMAKE_CURRENT_SOURCE_DIR}/bl616_bsp_i2c.c)
target_include_directories(aipi_bsp_i2c PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
