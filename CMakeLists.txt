cmake_minimum_required(VERSION 3.5)

if(NOT DEFINED ESP_PLATFORM)

project(scd30)

include(${PROJECT_SOURCE_DIR}/lib/scd30.cmake)

add_library(scd30 ${SCD30_SOURCES})

else()

idf_component_register(
    SRCS ${SCD30_SOURCES}
    INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/lib
)

endif()

