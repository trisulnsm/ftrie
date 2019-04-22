CC      = g++
CFLAGS  = -g -std=c++11 -fPIC -O0
LDFLAGS = -lpthread 
PREFIX = /usr/local

all: testapi 

libftrie.so: ftrie.o  GeoAPI.o  GeoDB.o StrTokenizer.o
	g++ -shared -o $@   ftrie.o GeoAPI.o GeoDB.o  StrTokenizer.o

libftrie.a: ftrie.o  GeoAPI.o  GeoDB.o StrTokenizer.o
	ar rcs $@ ftrie.o GeoAPI.o  GeoDB.o StrTokenizer.o

testapi: testapi.o  libftrie.a
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean 

clean:
	rm -f *.o *.a  testapi testmmdb libftrie.so  a.out


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
