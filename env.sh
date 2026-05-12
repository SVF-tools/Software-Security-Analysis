#!/bin/bash

# Get the repository directory and store it in PROJECTHOME.
# This lets the script work even if it is sourced from another directory.
PROJECTHOME=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)

# Detect the operating system type and store it in sysOS
sysOS=`uname -s`

# Define the LLVM version used by current SVF.
MajorLLVMVer=21
LLVMVer=${MajorLLVMVer}.1.0

# Set the home directories for LLVM and Z3
LLVMHome="llvm-${LLVMVer}.obj"
Z3Home="z3.obj"

# Change this to your SVF root directory
svf_root=$PROJECTHOME/../SVF

# Export the paths for the LLVM, Z3, and SVF directories
export LLVM_DIR=$svf_root/$LLVMHome
export Z3_DIR=$svf_root/$Z3Home
export SVF_DIR=$svf_root

# Update the PATH to include the binary directories for SVF, LLVM, and the project
if [ -d "$SVF_DIR/Debug-build/bin" ]; then
    export PATH=$SVF_DIR/Debug-build/bin:$PATH
fi
if [ -d "$SVF_DIR/Release-build/bin" ]; then
    export PATH=$SVF_DIR/Release-build/bin:$PATH
fi
export PATH=$LLVM_DIR/bin:$PATH
export PATH=$PROJECTHOME/build/bin:$PATH
export PATH=$PROJECTHOME/bin:$PATH

# Print the paths to the terminal for verification
echo "SVF_DIR="$SVF_DIR
echo "LLVM_DIR="$LLVM_DIR
echo "Z3_DIR="$Z3_DIR
