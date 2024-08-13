#!/bin/bash

#Set of instructions to simplify the build process of the fuse compiler
#BASICALLY

#1. Generate .ll file out of .fuse
#2. generate object file(.0) out of the generated .ll file above
#3. use clangd or gcc to generate binary/executable
#or
#4. use emscripten magic to generate wasm file & js wrapper(This is not being implemented yet)


VERSION=""

OS="$(uname)"

# Compile to wasm through emscripten (if necessary)

	# Generate executable
if [ "$OS" == "Linux" ]; then
  if [ "$VERSION" == "" ]; then
    for VERSION in {18..8} ""; do
      if command -v "llvm-as-$VERSION" >/dev/null 2>&1; then
        LLVM_AS="llvm-as-$VERSION"
        break
      fi
    done
  fi

  CLANGDCMD="clang-$VERSION"
  $CLANGDCMD "$1" -o "$2"
elif [ "$OS" == "Darwin" ]; then
  "clang" "$1" -o "$2"
fi

	if [ $? -ne 0 ]; then
		echo "clang command failed"
		exit 1
	fi


#File clean up
rm "$1"

# echo "INTERFUSE: Compilation successful ✅"
