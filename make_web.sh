
# Stop and exit on error
set -e

# FIXME: If we didn't have to source emscripten sdk this way, we
# Could change to building with an incremental build system such
# as Cmake, Raise, or even a Makefile.
# Setup Emscripten/WebAssembly SDK
source ../emsdk/emsdk_env.sh

cd unrar
make clean
make
