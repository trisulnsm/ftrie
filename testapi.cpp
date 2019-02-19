// Test the old v2 API 
#include <iostream>
#include <vector>
#include <stdexcept>
#include "GeoAPI.h"


using namespace std;

void runtest(GeoIP * pdb);

int main(int c, char ** v)
{
	if (c != 2 ) {
		throw std::domain_error("Usage : testoldapi   input-csv-blocks ");
	}


	GeoIP * pdb = GeoIP_open( v[1], 0 );
	runtest(pdb);

int i;
cin >> i;

	GeoIP_delete(pdb);

}

void runtest(GeoIP * pdb) 
{
	vector<string>  testips{
		"65.54.239.80",
		"0x4136ef50"
		"111.201.128.153",
		"192.168.3.255",
		"192.168.3.1",
		"192.168.3.0",
		"192.168.2.255",
		"192.168.212.82", 
		"49.206.114.59",
		"192.168.0.0",
		"192.167.255.255",
		"192.169.0.0",
		"192.168.255.255",
		"141.226.77.207",
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
