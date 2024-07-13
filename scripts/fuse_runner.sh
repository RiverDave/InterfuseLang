#!/bin/bash

#Set of instructions to simplify the build process of the fuse compiler
#BASICALLY

#1. Generate .ll file out of .fuse
#2. generate object file(.0) out of the generated .ll file above
#3. use clangd or gcc to generate binary/executable
#or
#4. use emscripten magic to generate wasm file & js wrapper

echo "INITIALIZING INTERFUSE COMPILER 🚀"

name=$1
rawname=$(basename "$1" .ll) # Remove suffix

objfile_name=$rawname.bc
# llc -filetype=asm -o="$objfile_name" "$1"
VERSION=""

OS="$(uname)"

# Compile to wasm through emscripten (if necessary)
if [ "$3" = "-w" ]; then

	# Platform specific commands, Im aware it might not be the best practice
	if [ "$OS" == "Linux" ]; then
		# Check for multiple versions of llvm
		for VERSION in {18..8} ""; do
			if command -v "llvm-as-$VERSION" >/dev/null 2>&1; then
				LLVM_AS="llvm-as-$VERSION"
				break
			fi
		done
	elif [ "$OS" == "Darwin" ]; then
		LLVM_AS="llvm-as" # I've found that llvm executables
		# Do not contain versioning in osx systems
	else
		echo "Unsupported operating system."
		exit 1
	fi

	# If LLVM_AS is not set, the required command was not found
	if [ -z "$LLVM_AS" ]; then
		echo "llvm-as command executed incorrectly."
		exit 1
	fi

	if [ -z "$VERSION" ]; then
		echo "Error finding LLVM version."
		exit 1
	fi

	# Generate bitcode
	$LLVM_AS "$1" -o "$rawname.bc"

	if [ $? -ne 0 ]; then
		echo "Error generating object file from .ll file"
		rm "$1"
		exit 1
	fi

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
fi


#File clean up
rm "$1"

echo "INTERFUSE: Compilation successful ✅"
