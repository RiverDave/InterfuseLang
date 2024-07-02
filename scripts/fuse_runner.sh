#!/bin/bash

#Set of instructions to simplify the build process of the fuse compiler
#BASICALLY

#1. Generate .ll file out of .fuse
#2. generate object file(.0) out of the generated .ll file above
#3. use clangd or gcc to generate binary/executable
#or
#4. use emscripten magic to generate wasm file & js wrapper


name=$1
rawname=$(basename "$1" .ll)

objfile_name=$rawname.bc
# llc -filetype=asm -o="$objfile_name" "$1"
llvm-as "$1" -o "$rawname.bc"

# Check if llc command was successful
if [ $? -ne 0 ]; then
    echo "Error generating object file from .ll file"
    rm "$1"
    exit 1
fi


# Compile to wasm through emscripten (if necessary)
if [ "$3" = "-w" ]; then


    emcc "$objfile_name" -sWASM=1 \
    -sEXPORTED_FUNCTIONS='["_main"]' \
    -sEXPORTED_RUNTIME_METHODS='["cwrap", "ccall"]' -sASSERTIONS=1 -sMODULARIZE=1 \
    -sENVIRONMENT=web -sEXPORT_ES6=1 -o "$2.js" 

    #
    # wasm-ld "$objfile" -o "$rawname".wasm --no-entry -allow-undefined --export-dynamic

    if [ $? -ne 0 ]; then
        echo "emcc command failed"
        exit 1
    fi

else
    # Generate executable
    clang "$objfile_name" -o "$2"
    if [ $? -ne 0 ]; then
        echo "clang command failed"
        exit 1
    fi
fi


# Run the executable(Not necessary tbh)
#./output

#File clean up
rm "$1"
rm "$objfile_name"

echo "INTERFUSE: Compilation successful ✅"
