add_library(common_clocking tbclock.cpp)

target_include_directories(common_clocking
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)