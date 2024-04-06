cmake_minimum_required(VERSION 3.26)
project(pourer)

set(CMAKE_CXX_STANDARD 17)

# Specify the path to the SDL2.framework
# set(SDL2_FRAMEWORK_PATH "/Library/Frameworks/SDL2.framework")

# Include the headers from the framework
include_directories(${SDL2_FRAMEWORK_PATH}/Headers)

# All headers shall be saved in here
set(INCLUDE_PATH "./Include")
include_directories(${INCLUDE_PATH})
set(SOURCES src/main.cpp)

add_executable(pourer ${SOURCES})

# # Link the SDL2.framework
# target_link_libraries(pourer ${SDL2_FRAMEWORK_PATH}/SDL2)

# Add compiler flags
target_compile_options(pourer PRIVATE -Wall -Wextra)
