
# Stop and exit on error
set -e

# FIXME: How do we do this inside the makefile?
# Setup Emscripten/WebAssembly SDK
source ../emsdk/emsdk_env.sh

g++ -O3 -std=c++14 file_to_header.cpp -o file_to_header
./file_to_header

cd unrar
make clean
make
