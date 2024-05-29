#!/bin/bash

# Check if an argument (expected to be a .c file) is provided
if [ -n "$1" ]; then
    # Extract the directory where the .c file is located
    dir=$(dirname "$1")

    # Extract the base name from the provided file and trim the .c extension
    file=$(basename "$1" .c)

    # Construct the full path for the output .ll file in the same directory as the .c file
    ll_file="$dir/$file.ll"

    # Use clang to compile the .c file to LLVM IR (.ll file), preserving variable names
    # and disabling optimizations that may discard value names
    clang -g -S -c -Xclang -disable-O0-optnone -fno-discard-value-names -emit-llvm "$1" -o "$ll_file"

    # Use LLVM's 'opt' tool to apply the mem2reg pass, which promotes memory to register
    # and outputs the transformed LLVM IR to the same .ll file
    opt -S -p=mem2reg "$ll_file" -o "$ll_file"
else
    # If no file is provided as an argument, output an error message
    echo "Please provide a .c file as input."
fi
