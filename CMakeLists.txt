cmake_minimum_required(VERSION 3.22.1)
project(interfuse)
find_package(LLVM REQUIRED CONFIG)



message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")


include_directories(${LLVM_INCLUDE_DIRS})

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

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
set(BUILD_PATH "${CMAKE_CURRENT_SOURCE_DIR}/build")

#Path stuff, automation
get_filename_component(FUSE_RUNNER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/scripts/fuse_runner.sh" ABSOLUTE)

add_definitions(-DFUSE_RUNNER_PATH="${FUSE_RUNNER_PATH}")


# Macos only

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(BISON_EXECUTABLE /opt/homebrew/opt/bison/bin/bison) # If not links to /usr/local/bin/bison (Really old version)
endif()

find_package(BISON 3.3 REQUIRED)
include_directories(${BISON_INCLUDE_DIRS})

BISON_TARGET(MyParser src/parser.y ${INCLUDE_PATH}/parser.cpp)



# link_directories(${LLVM_LIBRARY_DIR})
add_executable(interfuse ${SOURCES} ${BISON_MyParser_OUTPUTS})

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")



llvm_map_components_to_libnames(llvm_libs
    core
    executionengine
    mcjit
    # interpreter
    analysis
    native
    support 
    irreader
    # scalaropts
    # instcombine
    # transformutils
    # ipo
    # instrumentation
    # codegen
    # bitwriter
    # mcdisassembler
    # mcparser
    # bitreader
    # asmparser
    # target
    passes
  )


elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  # Temporal solution since trying to link libs same as above crashes on execution

execute_process(COMMAND llvm-config --libs all
                OUTPUT_VARIABLE llvm_libs)
message(STATUS "LLVM Libraries: ${llvm_libs}")
string(REPLACE "\n" " " llvm_libs ${llvm_libs})
string(STRIP "${llvm_libs}" llvm_libs)

endif()


target_include_directories(interfuse PRIVATE ${INCLUDE_PATH})



# Add compiler flags
if (CMAKE_BUILD_TYPE STREQUAL "Release")

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3") # Enable optimization
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O3")
  set(LLVM_ENABLE_ASSERTIONS OFF) # Disable assertions
  unset(CMAKE_EXPORT_COMPILE_COMMANDS) # Disable compile commands

elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")

  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  set(LLVM_ENABLE_ASSERTIONS ON)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address -g")

endif()

# install(TARGETS cloud_run_hello RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
target_link_libraries(interfuse PRIVATE ${llvm_libs})
target_compile_options(interfuse PRIVATE -Wall -Wextra)

include(GNUInstallDirs)
install(TARGETS interfuse RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
