#!/bin/bash

# Source the environment variables from the env.sh script
source ./env.sh

# Run cmake to configure the project with a Debug build type
cmake -DCMAKE_BUILD_TYPE=Debug .

# Compile the project using make
make -j4
