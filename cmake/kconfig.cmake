# Kconfig integration for CMake
# Based on ESP-IDF and Zephyr's approach

# Find Python for running menuconfig
find_package(Python3 REQUIRED COMPONENTS Interpreter)

# Set Kconfig root
set(KCONFIG_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/Kconfig" CACHE PATH "Kconfig root file")

# Set defaults file
set(KCONFIG_DEFAULTS "${CMAKE_BINARY_DIR}/Kconfig.defaults" CACHE PATH "Kconfig defaults")

# Set config output
set(KCONFIG_CONFIG "${CMAKE_BINARY_DIR}/Kconfig.config" CACHE PATH "Kconfig config output")
set(KCONFIG_CONFIG_HEADER "${CMAKE_BINARY_DIR}/kconfig_config.h" CACHE PATH "Kconfig config header")

# Menuconfig target
add_custom_target(menuconfig
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/kconfig.py
            --kconfig ${KCONFIG_ROOT}
            --config ${KCONFIG_CONFIG}
            --defaults ${KCONFIG_DEFAULTS}
            menuconfig
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running menuconfig..."
    VERBATIM
)

# Guiconfig target (graphical)
add_custom_target(guiconfig
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/kconfig.py
            --kconfig ${KCONFIG_ROOT}
            --config ${KCONFIG_CONFIG}
            --defaults ${KCONFIG_DEFAULTS}
            guiconfig
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running guiconfig..."
    VERBATIM
)

# Generate config header
function(kconfig_generate_config)
    # Create defaults from CMake cache
    file(WRITE ${KCONFIG_DEFAULTS} "# Auto-generated defaults from CMake\n")
    
    if(SCBB_CH224A)
        file(APPEND ${KCONFIG_DEFAULTS} "SCBB_CH224A=y\n")
    endif()
    if(SCBB_SHT3X)
        file(APPEND ${KCONFIG_DEFAULTS} "SCBB_SHT3X=y\n")
    endif()
    if(SCBB_WS2812)
        file(APPEND ${KCONFIG_DEFAULTS} "SCBB_WS2812=y\n")
    endif()
    if(SCBB_HXD039B2)
        file(APPEND ${KCONFIG_DEFAULTS} "SCBB_HXD039B2=y\n")
    endif()
    if(SCBB_USE_BSP)
        file(APPEND ${KCONFIG_DEFAULTS} "SCBB_USE_BSP=y\n")
    endif()
    
    # Run conf to generate config
    if(EXISTS ${KCONFIG_CONFIG})
        execute_process(
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/kconfig.py
                    --kconfig ${KCONFIG_ROOT}
                    --config ${KCONFIG_CONFIG}
                    --defaults ${KCONFIG_DEFAULTS}
                    --header ${KCONFIG_CONFIG_HEADER}
                    genconfig
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            RESULT_VARIABLE result
        )
        
        if(result EQUAL 0 AND EXISTS ${KCONFIG_CONFIG_HEADER})
            # Read generated config
            file(READ ${KCONFIG_CONFIG_HEADER} config_content)
            
            # Parse config values
            string(REGEX MATCH "CONFIG_SCBB_CH224A=([0-9]+)" match "${config_content}")
            if(match)
                set(SCBB_CH224A ${CMAKE_MATCH_1} PARENT_SCOPE)
            endif()
            
            string(REGEX MATCH "CONFIG_SCBB_SHT3X=([0-9]+)" match "${config_content}")
            if(match)
                set(SCBB_SHT3X ${CMAKE_MATCH_1} PARENT_SCOPE)
            endif()
            
            string(REGEX MATCH "CONFIG_SCBB_WS2812=([0-9]+)" match "${config_content}")
            if(match)
                set(SCBB_WS2812 ${CMAKE_MATCH_1} PARENT_SCOPE)
            endif()
            
            string(REGEX MATCH "CONFIG_SCBB_HXD039B2=([0-9]+)" match "${config_content}")
            if(match)
                set(SCBB_HXD039B2 ${CMAKE_MATCH_1} PARENT_SCOPE)
            endif()
            
            string(REGEX MATCH "CONFIG_SCBB_USE_BSP=([0-9]+)" match "${config_content}")
            if(match)
                set(SCBB_USE_BSP ${CMAKE_MATCH_1} PARENT_SCOPE)
            endif()
            
            # Parse BSP headers
            string(REGEX MATCH "CONFIG_SCBB_BSP_I2C_HEADER=\"([^\"]+)\"" match "${config_content}")
            if(match)
                set(SCBB_BSP_I2C_HEADER ${CMAKE_MATCH_1} PARENT_SCOPE)
            endif()
            
            string(REGEX MATCH "CONFIG_SCBB_BSP_UART_HEADER=\"([^\"]+)\"" match "${config_content}")
            if(match)
                set(SCBB_BSP_UART_HEADER ${CMAKE_MATCH_1} PARENT_SCOPE)
            endif()
            
            string(REGEX MATCH "CONFIG_SCBB_BSP_SPI_HEADER=\"([^\"]+)\"" match "${config_content}")
            if(match)
                set(SCBB_BSP_SPI_HEADER ${CMAKE_MATCH_1} PARENT_SCOPE)
            endif()
            
            string(REGEX MATCH "CONFIG_SCBB_BSP_GPIO_HEADER=\"([^\"]+)\"" match "${config_content}")
            if(match)
                set(SCBB_BSP_GPIO_HEADER ${CMAKE_MATCH_1} PARENT_SCOPE)
            endif()
            
            string(REGEX MATCH "CONFIG_SCBB_BSP_DELAY_HEADER=\"([^\"]+)\"" match "${config_content}")
            if(match)
                set(SCBB_BSP_DELAY_HEADER ${CMAKE_MATCH_1} PARENT_SCOPE)
            endif()
        endif()
    endif()
endfunction()

# Export config as compile definitions
function(kconfig_export_config target)
    if(SCBB_CH224A)
        target_compile_definitions(${target} PUBLIC SCBB_CH224A_ENABLED=1)
    endif()
    if(SCBB_SHT3X)
        target_compile_definitions(${target} PUBLIC SCBB_SHT3X_ENABLED=1)
    endif()
    if(SCBB_WS2812)
        target_compile_definitions(${target} PUBLIC SCBB_WS2812_ENABLED=1)
    endif()
    if(SCBB_HXD039B2)
        target_compile_definitions(${target} PUBLIC SCBB_HXD039B2_ENABLED=1)
    endif()
    
    if(NOT SCBB_USE_BSP)
        target_compile_definitions(${target} PRIVATE
            SCBB_BSP_I2C_HEADER="${SCBB_BSP_I2C_HEADER}"
            SCBB_BSP_UART_HEADER="${SCBB_BSP_UART_HEADER}"
            SCBB_BSP_SPI_HEADER="${SCBB_BSP_SPI_HEADER}"
            SCBB_BSP_GPIO_HEADER="${SCBB_BSP_GPIO_HEADER}"
            SCBB_BSP_DELAY_HEADER="${SCBB_BSP_DELAY_HEADER}"
        )
    endif()
endfunction()
