cmake_minimum_required(VERSION 3.26)
project(pourer)
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

set(CMAKE_CXX_STANDARD 17)


# All headers shall be saved in here
# set(INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SOURCES
            src/main.cpp
            src/Lexer.cpp
            src/Token.cpp
            include/Lexer.h
            include/Token.h
            include/AST.h
            src/parser.y

)

find_package(BISON REQUIRED)
BISON_TARGET(MyParser src/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)


add_executable(pourer ${SOURCES} ${BISON_MyParser_OUTPUTS})

# target_include_directories(pourer PRIVATE ${INCLUDE_PATH})
llvm_map_components_to_libnames(llvm_libs support core irreader)


# Add compiler flags
target_compile_options(pourer PRIVATE -Wall -Wextra)
target_link_libraries(pourer ${llvm_libs})
