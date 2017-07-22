# unrar.js

A port of UnRAR to JavaScript using WebAssembly.

http://www.rarlab.com/rar/unrarsrc-5.5.3.tar.gz

# !!!!!!!!!!!!!!!!!!!!!!
# This project is just getting started, and does not work yet!
# !!!!!!!!!!!!!!!!!!!!!!

# Build:
```bash
./make_web.sh
```

# Run:
```bash
python3 -m http.server 8000
```

# FIXME:
* Make a simple library for passing data between C++ and JS
* Remove vector, string to make wasm smaller
* Replace progress printing to console with callbacks
* Replace the Emscripten generated file with a simple example
* Passing between C++ and JavaScript is uber slow. As we pass it one byte at a time.
* It blocks when it does anything. Add functions for Web Workers.
