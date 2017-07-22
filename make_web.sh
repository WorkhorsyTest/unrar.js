
# Stop and exit on error
set -e

# FIXME: How do we do this inside the makefile?
# Setup Emscripten/WebAssembly SDK
source ../emsdk/emsdk_env.sh

rm -f source/*.o
rm -f unrar.js
rm -f unrar.js.map
rm -f unrar.wasm
rm -f unrar.wast
cd source
make clean
make
