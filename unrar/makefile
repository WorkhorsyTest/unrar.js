

# WebAssembly using Emscripten
CXX=em++
CXXFLAGS=-O2
LIBFLAGS=
DEFINES=-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE
LDFLAGS=-pthread -s WASM=1

##########################

COMPILE=$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(DEFINES)
LINK=$(CXX)

WHAT=UNRAR

UNRAR_OBJ=filestr.o recvol.o rs.o scantree.o qopen.o
LIB_OBJ=filestr.o scantree.o dll.o qopen.o

OBJECTS=rar.o strlist.o strfn.o pathfn.o smallfn.o global.o file.o filefn.o filcreat.o \
	archive.o arcread.o unicode.o system.o isnt.o crypt.o crc.o rawread.o encname.o \
	resource.o match.o timefn.o rdwrfn.o consio.o options.o errhnd.o rarvm.o secpassword.o \
	rijndael.o getbits.o sha1.o sha256.o blake2s.o hash.o extinfo.o extract.o volume.o \
  list.o find.o unpack.o headers.o threadpool.o rs16.o cmddata.o ui.o

.cpp.o:
	$(COMPILE) -D$(WHAT) -c $<

all:	unrar.html

clean:
	@rm -f *.o *.bak *~
	@rm -f unrar
	@rm -f unrar.js
	@rm -rf unrar.html

unrar.html:	clean $(OBJECTS) $(UNRAR_OBJ)
	@rm -f unrar.html
	$(LINK) -o unrar.html $(LDFLAGS) $(OBJECTS) $(UNRAR_OBJ) $(LIBS)

lib:	WHAT=RARDLL
lib:	CXXFLAGS+=$(LIBFLAGS)
lib:	clean $(OBJECTS) $(LIB_OBJ)
	@rm -f libunrar.so
	@rm -f libunrar.a
	$(LINK) -shared -o libunrar.so $(LDFLAGS) $(OBJECTS) $(LIB_OBJ)
	$(AR) rcs libunrar.a $(OBJECTS) $(LIB_OBJ)
