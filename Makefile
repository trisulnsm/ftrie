CC      = g++
CFLAGS  = -g -std=c++11 -fPIC -O2
LDFLAGS = -lpthread 
PREFIX = /usr/local

all: testapi 

libftrie.a: ftrie.o  GeoAPI.o  GeoDB.o
	ar rcs $@ ftrie.o GeoAPI.o  GeoDB.o

testapi: testapi.o  libftrie.a
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean 

clean:
	rm -f *.o *.a  testapi


.PHONY: install
install: libftrie.a
	mkdir -p $(DESTDIR)$(PREFIX)/lib
	mkdir -p $(DESTDIR)$(PREFIX)/include
	cp libftrie.a $(DESTDIR)$(PREFIX)/lib/$(alib)
	cp GeoAPI.h $(DESTDIR)$(PREFIX)/include/

.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/lib/libftrie.a
	rm -f $(DESTDIR)$(PREFIX)/include/GeoAPI.h
