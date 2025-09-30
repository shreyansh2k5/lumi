#!/usr/bin/env bash
# exit on error
set -o errexit

echo "Installing dependencies..."

# Update package lists and install C build tools, git, and required libraries
# libcurl is for HTTP requests, libjansson is for JSON parsing.
sudo apt-get update
sudo apt-get install -y build-essential git libcurl4-openssl-dev libjansson-dev

echo "Cloning and installing Concord..."

# Concord is a C library for the Discord API. We need to clone it and build it from source.
git clone https://github.com/Cog-Creators/concord
cd concord
# Configure the build to install into /usr/local which is a standard location
./configure --prefix=/usr/local
# Compile and install the library
make
sudo make install

# Add the new library path to the dynamic linker's cache so our program can find it
sudo ldconfig

echo "Moving back to base directory..."
cd ..

echo "Running make to compile the bot..."
# Now that all dependencies are installed, compile our C code.
make

echo "Build finished!"

