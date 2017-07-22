

# WebAssembly using Emscripten
CXX=em++
CXXFLAGS=-g -O3 -std=c++11
#CXXFLAGS=-g4 -O0 -std=c++11
DEFINES=-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE
LDFLAGS=-pthread --bind -s WASM=1 -s DEMANGLE_SUPPORT=1 -s FORCE_FILESYSTEM=1 -s ASSERTIONS=2 -s ALLOW_MEMORY_GROWTH=1 -s ABORTING_MALLOC=0

##########################

UNRAR_OBJ=filestr.o recvol.o rs.o scantree.o qopen.o

OBJECTS=rar.o strlist.o strfn.o pathfn.o smallfn.o global.o file.o filefn.o filcreat.o \
	archive.o arcread.o unicode.o system.o isnt.o crypt.o crc.o rawread.o encname.o \
	resource.o match.o timefn.o rdwrfn.o consio.o options.o errhnd.o rarvm.o secpassword.o \
	rijndael.o getbits.o sha1.o sha256.o blake2s.o hash.o extinfo.o extract.o volume.o \
  list.o find.o unpack.o headers.o threadpool.o rs16.o cmddata.o ui.o


all:	unrar.js

clean:
	@rm -f *.o
	@rm -f unrar.js
	@rm -f unrar.js.map
	@rm -f unrar.wasm
	@rm -f unrar.wast
	@rm -f unrar.wasm.map

unrar.js:	clean $(OBJECTS) $(UNRAR_OBJ)
	$(CXX) $(CXXFLAGS) -o ../unrar.js $(LDFLAGS) $(DEFINES) $(OBJECTS) $(UNRAR_OBJ)
