cmake_minimum_required(VERSION 3.26)
project(pourer)

set(CMAKE_CXX_STANDARD 17)


# All headers shall be saved in here
# set(INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SOURCES src/main.cpp
            src/Lexer.cpp
            src/Token.cpp
  )



add_executable(pourer ${SOURCES})

# target_include_directories(pourer PRIVATE ${INCLUDE_PATH})


# Add compiler flags
target_compile_options(pourer PRIVATE -Wall -Wextra)
