cmake_minimum_required(VERSION 3.26)
project(interfuse)
find_package(LLVM REQUIRED CONFIG)
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
            include/intercore.hpp
            src/IR.cpp
            include/internals.hpp
            src/IteratorInterface.cpp
            src/parser.y
            src/FuseHandler.cpp

)

message(STATUS "SOURCES: ${SOURCES}")

set(INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include")

#Path stuff, automation
get_filename_component(FUSE_RUNNER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/scripts/fuse_runner.sh" ABSOLUTE)

add_definitions(-DFUSE_RUNNER_PATH="${FUSE_RUNNER_PATH}")


find_package(BISON REQUIRED)
BISON_TARGET(MyParser src/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)




link_directories(${LLVM_LIBRARY_DIR})



set(LLVM_COMPONENTS # Not sure how accurate this is
    core
    executionengine
    mcjit
    interpreter
    analysis
    native
    scalaropts
    instcombine
    transformutils
    ipo
    instrumentation
    codegen
    bitwriter
    mcdisassembler
    mcparser
    bitreader
    asmparser
    support
    target
    passes
)

llvm_map_components_to_libnames(llvm_libs ${LLVM_COMPONENTS})


add_executable(interfuse ${SOURCES} ${BISON_MyParser_OUTPUTS})
target_include_directories(interfuse PRIVATE ${INCLUDE_PATH})


# Add compiler flags
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(LLVM_ENABLE_ASSERTIONS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address -g")

target_link_libraries(interfuse ${llvm_libs} ${targets})
target_compile_options(interfuse PRIVATE -Wall -Wextra)
