CC      = g++
CFLAGS  = -g -std=c++11 -fPIC -O2
LDFLAGS = -lpthread 
PREFIX = /usr/local

all: testcountry  testasn testoldapi 

libftrie.a: ftrie.o  MaxmindCountryDB.o MaxmindASNDB.o NewMaxmindDB.o 
	ar rcs $@ ftrie.o MaxmindASNDB.o MaxmindCountryDB.o NewMaxmindDB.o 

testcountry: testcountry.o  libftrie.a 
	$(CC) -o $@ $^ $(LDFLAGS)

testasn: testasn.o  libftrie.a
	$(CC) -o $@ $^ $(LDFLAGS)

testoldapi: testoldapi.o  libftrie.a
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean 

clean:
	rm -f *.o *.a  testasn testcountry  testoldapi


.PHONY: install
install: libftrie.a
	mkdir -p $(DESTDIR)$(PREFIX)/lib
	mkdir -p $(DESTDIR)$(PREFIX)/include
	cp libftrie.a $(DESTDIR)$(PREFIX)/lib/$(alib)
	cp MaxmindASNDB.h $(DESTDIR)$(PREFIX)/include/
	cp MaxmindCountryDB.h $(DESTDIR)$(PREFIX)/include/
	cp NewMaxmindDB.h $(DESTDIR)$(PREFIX)/include/

.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/lib/libftrie.a
	rm -f $(DESTDIR)$(PREFIX)/include/MaxmindASNDB.h
	rm -f $(DESTDIR)$(PREFIX)/include/MaxmindCountryDB.h
	rm -f $(DESTDIR)$(PREFIX)/include/NewMaxmindDB.h
