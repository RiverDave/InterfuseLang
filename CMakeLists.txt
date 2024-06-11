cmake_minimum_required(VERSION 3.26)
project(pourer)
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})
set (CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 17)


set(SOURCES
            src/main.cpp
            src/Lexer.cpp
            src/Token.cpp
            include/Lexer.h
            include/Token.h
            include/AST.h
            include/IR.h
            include/KaleidoscopeJIT.h
            include/Intercore.hpp
            src/IR.cpp
            include/internals.hpp
            src/IteratorInterface.cpp
            src/parser.y

)

# set(SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include")

#Path stuff, automation
get_filename_component(FUSE_RUNNER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/scripts/fuse_runner.sh" ABSOLUTE)

add_definitions(-DFUSE_RUNNER_PATH="${FUSE_RUNNER_PATH}")


find_package(BISON REQUIRED)
BISON_TARGET(MyParser src/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)




link_directories(${LLVM_LIBRARY_DIR})


execute_process(COMMAND llvm-config --libs all
                OUTPUT_VARIABLE llvm_libraries)
string(REPLACE "\n" " " llvm_libraries ${llvm_libraries})
string(STRIP ${llvm_libraries} llvm_libraries)

add_executable(pourer ${SOURCES} ${BISON_MyParser_OUTPUTS})
target_include_directories(pourer PRIVATE ${INCLUDE_PATH})


# Add compiler flags
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(LLVM_ENABLE_ASSERTIONS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address -g")

target_link_libraries(pourer ${llvm_libraries} ${targets})
target_compile_options(pourer PRIVATE -Wall -Wextra)
