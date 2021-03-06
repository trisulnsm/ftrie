# ftrie

> UPDATE 9-APR-2019 :   Added a LuaJIT FFI wrapper ftrie.lua 
> UPDATE 20-MAR-2019 :   Can now read aspath dumps from quagga

Library to lookup IPv4 address with focus on speed. See _ftrie.h_ for the backing datastructure - a modified version of a RADIX Trie 

Currently supports the [Maxmind GeoLite2 CSV databases](https://dev.maxmind.com/geoip/geoip2/geolite2/). You can support any other CSV database by supplying the custom CSV parsing code in _GeoAPI.cpp_

> The primary use case is network analytics that requires a very high speed lookup. We found the MMDB format and the libmaxmind API too slow to use in our high speed network analytics application -   Trisul Network Analytics https://trisul.org 

## Using the API 

To install this library follow the usual process. The API file is installed in `/usr/local/include/GeoAPI.h`


```
make  && make install
link your app with -lftrie 
```

#### LUA and shared library 
To build a shared library, do `make clean && make libftrie.so` 

You can also find a LuaJIT FFI ftrie.lua and a test_ftrie.lua 

```lua
vivek@viveku14:~/ft00/ftrie$ luajit test_ftrie.lua 
testing dotted
10796   Charter Communications Inc
testing trisulkey
0x4A3E4848
10796   Charter Communications Inc

```


### API calls 

The API features  `GeoIP_by_ipnum()`  that returns a _Key_  and a _Label_  for a 32-Bit ip number. The _Key_ is a short string such as Country Code "US/UK/IN" and _Label_ is the expanded version such as "United States".  See GeoAPI.h for looking up by ip address and trisul key format.

````

	// open the supported CSV files (all IPv4 from MaxMind are supported) 
	GeoIP * pdb = GeoIP_open( "GeoLite2-City-Blocks-IPv4.csv", 0 );


	// always lookup by IP number  - eg 192.168.1.2 = 0xC0A80102 
	string k,l;
	if (GeoIP_by_ipnum(pdb, 0xC0A80102, &k, &l) ) {
		cout << "Matching IP " << ip << "  key = "  << k << " label= " << l << endl;
	} else {
		cout << "Matching IP " << ip << "  Not found" << endl;
	}
	

	GeoIP_delete(pdb);

````

#### GeoIP_open()  filename usage note

The filename passed to `GeoIP_open()` determines the type of CSV file. Currently ftrie supports the Maxmind GeoLite2 CSV files, hence the filenames must match those names.


### Shared library 

Use the target libftrie.so `make libftrie.so` 


## Benchmark

The focus is on speed - 

On a i3/3.3Ghz/3GB Cache/4GB RAM/7200 SATA


````
vivek@viveku14:~/ftrie$ ./testapi /tmp/GeoLite2-Country-Blocks-IPv4.csv 
...
Matching IP 1.0.24.18  key = JP label= Japan
Matching IP 1.0.31.255  key = JP label= Japan

Finished 10M lookups in 1940.54 milliseconds  at rate 5.1532e+09 per second

````

That is a blazing **5.15M totally random lookups/second**  !! 



### Comparison with  libmaxminddb

This library was written to overcome the slow and cumbersome new API from GeoLite2 

Here are the numbers for the libmaxmind API. Note we are not even accessing the 
fields, just stopping at `getentrydatalist(..)` and also used the `sockaddr` API to 
eliminate the performance impact of  conversion of dotted decimal string to a 32bit IP number.

````
vivek@viveku14:~/ftrie$ g++  testmmdb.cpp   -lmaxminddb -O2 -o testmmdb -std=c++11
vivek@viveku14:~/ftrie$ ./testmmdb /tmp/GeoLite2-City.mmdb 
Finished 10M lookups in 25295.8 milliseconds  at rate 3.95323e+08 per second
````

So we have a *12X improvement* !!  

There is however a small memory tradeoff. 
 - The libmaxminddb  uses 75MB for the CITY database, 
 - This libftrie uses 206MB for the CITY database

## TODO 

Support IPv6 
Reduce memory use further 

