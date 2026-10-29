cmake_minimum_required(VERSION 3.22)

# THESE TWO LINES ARE CRITICAL AND MUST COME BEFORE project()
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

project(EnergyRune_H7 C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Define the target MCU flags
set(MCU_FLAGS "-mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard")

# Append them to the existing compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MCU_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MCU_FLAGS}")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} ${MCU_FLAGS}")

# Don't forget to add them to the linker flags as well
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${MCU_FLAGS}")

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# Collect all application sources
file(GLOB_RECURSE APP_SOURCES
    ${CMAKE_SOURCE_DIR}/0Task/*.c
    ${CMAKE_SOURCE_DIR}/0ws_graph/*.c
   # ${CMAKE_SOURCE_DIR}/1Module__Algorithm/*.c
    ${CMAKE_SOURCE_DIR}/2Driver/*.c
    ${CMAKE_SOURCE_DIR}/Core/Src/*.c
)
list(REMOVE_ITEM APP_SOURCES ${CMAKE_SOURCE_DIR}/Core/Src/system_stm32h7xx.c)

# --- Application Include Directories ---
set(APP_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/0Task
    ${CMAKE_SOURCE_DIR}/0ws_graph
    ${CMAKE_SOURCE_DIR}/1Module__Algorithm
    ${CMAKE_SOURCE_DIR}/2Config
    ${CMAKE_SOURCE_DIR}/2Driver
    ${CMAKE_SOURCE_DIR}/Core/Inc
)

# --- Create the executable ---
add_executable(${CMAKE_PROJECT_NAME}
    ${APP_SOURCES}
    ${CMAKE_SOURCE_DIR}/startup_stm32h743xx.s
)

# --- Include directories ---
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${APP_INCLUDE_DIRS}
)

# --- Compile definitions ---
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    USE_HAL_DRIVER
    STM32H743xx
    USE_PWR_LDO_SUPPLY
    $<$<CONFIG:Debug>:DEBUG>
)

# --- Linker script ---
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
    -T "${CMAKE_SOURCE_DIR}/STM32H743XX_FLASH.ld"
)

# --- STM32CubeMX sub-build (HAL Drivers + MX sources) ---
add_subdirectory(${CMAKE_SOURCE_DIR}/cmake/stm32cubemx)

target_link_libraries(${CMAKE_PROJECT_NAME}
    STM32_Drivers
    stm32cubemx
    m
)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
    SUFFIX ".elf"
)

# --- Post-build: generate .hex and .bin ---
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex "${CMAKE_PROJECT_NAME}.elf" "${CMAKE_PROJECT_NAME}.hex"
    COMMAND ${CMAKE_OBJCOPY} -O binary "${CMAKE_PROJECT_NAME}.elf" "${CMAKE_PROJECT_NAME}.bin"
#    COMMAND ${CMAKE_SIZE} "${CMAKE_PROJECT_NAME}.elf"
    COMMENT "Generating .hex and .bin files"
)
