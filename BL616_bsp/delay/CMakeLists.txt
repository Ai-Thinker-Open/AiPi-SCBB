add_library(aipi_bsp_delay STATIC ${CMAKE_CURRENT_SOURCE_DIR}/bl616_bsp_delay.c)
target_include_directories(aipi_bsp_delay PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
