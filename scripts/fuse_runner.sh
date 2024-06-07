#!/bin/bash

#Set of instructions to simplify the build process of the fuse compiler
#BASICALLY

#1. Generate .ll file out ouf .fuse
#2. generate object file(.0) out of the generated .ll file above
#3. use clangd or gcc to generate binary/executable


objfile=$(basename "$1" .ll).o
llc -filetype=obj -o="$objfile" "$1"


# generate executable
clang "$objfile" -o "$2"


# Step 4: Run the executable(Not necessary tbh)
#./output

#Step 5: Delete the .ll and .o files
rm "$1"
rm "$objfile"
