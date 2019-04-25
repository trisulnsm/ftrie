// Test the old v2 API 
#include <iostream>
#include <vector>
#include <stdexcept>
#include "GeoAPI.h"
#include <chrono>
#include <random>


using namespace std;

void runtest(GeoIP * pdb);
void runbenchmark(GeoIP * pdb);

int main(int c, char ** v)
{
	if (c != 2 ) {
		throw std::domain_error("Usage : testapi   input-csv-blocks ");
	}


	GeoIP * pdb = GeoIP_open( v[1], 0 );
	runtest(pdb);


	// Enter any number to exit 
	// to hold DB in memory for perftools
	cout << "Enter any number to end test : " << flush;
	int i;
	cin >> i;

	GeoIP_delete(pdb);

}

void runtest(GeoIP * pdb) 
{
	vector<string>  testips{
		"61.95.168.122",
		"118.185.188.11",
		"95.101.2.25",
		"65.54.239.80",
		"111.201.128.153",
		"192.168.3.255",
		"192.168.3.1",
		"192.168.3.0",
		"192.168.2.255",
		"192.168.212.82", 
		"49.206.114.59",
		"49.207.48.59",
		"49.204.211.254",
		"192.168.0.0",
		"192.167.255.255",
		"192.169.0.0",
		"192.168.255.255",
		"141.226.77.207",
		"194.14.216.22",
		"1.0.15.255",
		"1.0.15.255",
		"1.0.16.0",
		"1.0.24.18",
		"1.0.31.255"
	};

	for (auto ip : testips) {
		int a,b,c,d;
		if (sscanf(ip.c_str(),"%d.%d.%d.%d", &a,&b,&c,&d)==4) {
			uint32_t ipnum  = (a<<24) | (b << 16) | (c << 8)  | d ;
			const char * k, * l;
			if (GeoIP_by_ipnum(pdb, ipnum, &k, &l) ) {
					cout << "Matching IP " << ip << "  key = "  << k << " label= " << l << endl;
			} else {
					cout << "Matching IP " << ip << "  Not found" << endl;
			}
		}
	}

}

void	runbenchmark(GeoIP * pdb) 
{

	// 10 M random lookup performance
	std::random_device r;
	std::default_random_engine e1(r());
	std::uniform_int_distribution<uint32_t> uniform_dist(0, 0xFFFFFFFF);
	auto start = chrono::steady_clock::now();
	int ok=0,fail=0;
	for (size_t i =0; i < 10000000; i ++) 
	{
		uint32_t ipnum  = uniform_dist(e1);
		const char * k, * l;
		if (GeoIP_by_ipnum(pdb, ipnum, &k, &l) ) {
			++ok;
		} else {
			++fail;
		}
	}
	auto end  = chrono::steady_clock::now();
	auto ms   = chrono::duration <double, milli> (end-start).count() ;

	cout << "Finished 10M lookups in " << ms << " milliseconds" << "  at rate " << (double) 1e+10/(ms/1000) << " per second" << endl;
	cout << "lookup ok = " << ok << " , notfound = " << fail << endl;

}
