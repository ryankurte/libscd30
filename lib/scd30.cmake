
# List sources
set(SCD30_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/source/scd30.c
)


# Allow headers to be located
include_directories(${CMAKE_CURRENT_LIST_DIR})

# Add the SCD30 library
add_library(scd30 ${SCD30_SOURCES})

