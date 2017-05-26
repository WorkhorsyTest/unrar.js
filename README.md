# unrar.js

A port of UnRAR to JavaScript using WebAssembly.

http://www.rarlab.com/rar/unrarsrc-5.5.3.tar.gz

# !!!!!!!!!!!!!!!!!!!!!!
# This project is just getting started, and does not work yet!
# !!!!!!!!!!!!!!!!!!!!!!

## FIXME:
* Passing the rar file from JS to C++ is user slow, as we pass it one byte at a time.
* RAR does not let go of file handles. So there is no way to see if files are uncompressed correctly.
* It uses a ton or RAM, and never frees it. Like 1GB to open a 50KB file.
* It blocks when it does anything. Add functions for Web Workers.
