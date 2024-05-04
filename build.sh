#!/bin/bash

# Check if npm is installed by trying to locate its command
if ! command -v npm &> /dev/null
then
    echo "npm is not installed. Please install npm and try again."
    exit 1
fi

# Install the 'svf-lib' library using npm, with the installation prefix set to the user's home directory
npm i --silent svf-lib --prefix ${HOME}

# Check if npm install was successful
if [ $? -ne 0 ]; then
    echo "npm install failed. Please check the error message above."
    exit 1
fi

# Source the environment variables from the env.sh script
source ./env.sh

# Run cmake to configure the project with a Debug build type
cmake -DCMAKE_BUILD_TYPE=Debug .

# Compile the project using make
make
