CC      = g++
CFLAGS  = -g -std=c++11 -fPIC
LDFLAGS = -lpthread -lre2 

all: testcountry  testasn

libftrie.a: ftrie.o  MaxmindCountryDB.o MaxmindASNDB.o
	ar rcs $@ ftrie.o MaxmindASNDB.o MaxmindCountryDB.o

testcountry: testcountry.o  libftrie.a 
	$(CC) -o $@ $^ $(LDFLAGS)

testasn: testasn.o  libftrie.a
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean 

clean:
	rm -f *.o *.a  testasn testcountry 

