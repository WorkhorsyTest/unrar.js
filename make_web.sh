
# Stop and exit on error
set -e

# FIXME: How do we do this inside the makefile?
# Setup Emscripten/WebAssembly SDK
source ../emsdk/emsdk_env.sh

cd unrar
make clean
make
