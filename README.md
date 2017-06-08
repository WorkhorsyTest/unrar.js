# unrar.js

A port of UnRAR to JavaScript using WebAssembly.

http://www.rarlab.com/rar/unrarsrc-5.5.3.tar.gz

# !!!!!!!!!!!!!!!!!!!!!!
# This project is just getting started, and does not work yet!
# !!!!!!!!!!!!!!!!!!!!!!

## FIXME:
* Make it return a list of files to JS by callback
* Replace progress printing to console with callbacks
* Have it generate a list of links to click and extract to images
* Replace the Emscripten generated file with a simple example
* Remove copies of files stored in arrays, and virtual file system, and object urls.
* Passing between C++ and JavaScript is uber slow. As we pass it one byte at a time.
* It uses a ton or RAM, and never frees it. Like 1GB to open a 50KB file.
* It blocks when it does anything. Add functions for Web Workers.
