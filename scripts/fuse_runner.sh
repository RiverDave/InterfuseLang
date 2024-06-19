#!/bin/bash

#Set of instructions to simplify the build process of the fuse compiler
#BASICALLY

#1. Generate .ll file out ouf .fuse
#2. generate object file(.0) out of the generated .ll file above
#3. use clangd or gcc to generate binary/executable


objfile=$(basename "$1" .ll).o
llc -filetype=obj -o="$objfile" "$1"

# Check if llc command was successful
if [ $? -ne 0 ]; then
    echo "Error generating object file from .ll file"
    rm "$1"
    exit 1
fi


# generate executable
clang "$objfile" -o "$2"

if [ $? -ne 0 ]; then
    echo "clang command failed"
    exit 1
fi


# Step 4: Run the executable(Not necessary tbh)
#./output

#Step 5: Delete the .ll and .o files
rm "$1"
rm "$objfile"
