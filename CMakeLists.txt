cmake_minimum_required(VERSION 3.0)
project(udp_pkt_loss_sim)

set(CMAKE_CXX_STANDARD 17)

# Check if argparse submodule exists, if not, download it
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/argparse/CMakeLists.txt")
    find_package(Git REQUIRED)
    message(STATUS "Fetching argparse submodule")
    execute_process(
        COMMAND ${GIT_EXECUTABLE} clone --recursive https://github.com/p-ranav/argparse.git ${CMAKE_CURRENT_SOURCE_DIR}/argparse
        RESULT_VARIABLE GIT_RESULT
    )
    if(NOT GIT_RESULT EQUAL "0")
        message(FATAL_ERROR "Failed to fetch argparse submodule. Git returned ${GIT_RESULT}")
    endif()
endif()

# Add the executable target
add_executable(udp_pkt_loss_sim main.cpp)

# Include argparse library
add_subdirectory(argparse)
target_include_directories(udp_pkt_loss_sim PRIVATE argparse/include/argparse)

# Link libraries if needed
# target_link_libraries(udp_pkt_loss_sim ...)

