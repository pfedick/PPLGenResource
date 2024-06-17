BUILD_NUMBER := $(shell git rev-list --all --count)
BUILDDATE := $(shell date '+%Y%m%d')

INCLUDE	= -Iinclude
CFLAGS	= -ggdb -O2 -Wall  $(INCLUDE) $(EXTRA_CFLAGS) `ppl7-config --cflags`
LIB		= -L/usr/local/lib  `ppl7-config --libs`  -lstdc++
PROGRAM	= pplgenresource

LIBDEP	:= $(shell ppl7-config --ppllib release)

OBJECTS=compile/main.o compile/genresource.o
	

$(PROGRAM): compile $(OBJECTS) $(LIBDEP)
	$(CXX) -o $(PROGRAM) $(OBJECTS) $(CFLAGS) $(LIB)
	-chmod 755 $(PROGRAM)

all: $(PROGRAM)

compile:
	-mkdir -p compile

clean:
	-rm -f compile/*.o $(PROGRAM) *.core
	
install: $(PROGRAM)
	-cp $(PROGRAM) /usr/local/bin

compile/main.o: src/main.cpp include/genresource.h Makefile
	$(CXX) -o compile/main.o -c src/main.cpp $(CFLAGS)

compile/genresource.o: src/genresource.cpp include/genresource.h Makefile
	$(CXX) -o compile/genresource.o -c src/genresource.cpp $(CFLAGS)
	
