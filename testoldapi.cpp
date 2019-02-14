// Test the old v2 API 
#include <iostream>
#include <vector>
#include "NewMaxmindDB.h"
#include <re2/re2.h>


using namespace std;

void runtest(GeoIP * pdb);

int main(int c, char ** v)
{
	if (c != 2 ) {
		throw std::domain_error("Usage : testoldapi   input-csv-blocks ");
	}


	GeoIP * pdb = GeoIP_open( v[1], 0 );
	runtest(pdb);

	GeoIP_delete(pdb);

}

void runtest(GeoIP * pdb) 
{
	vector<string>  testips{
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
	RE2   regex("(\\d+)\\.(\\d+)\\.(\\d+)\\.(\\d+)" );
	for (auto ip : testips) {
		int a,b,c,d;
		if (RE2::PartialMatch(ip, regex, &a,&b,&c,&d)) {
			uint32_t ipnum  = (a<<24) | (b << 16) | (c << 8)  | d ;
			cout << "Matching IP " << ip << "  value = " 
					<< GeoIP_country_code_by_ipnum(pdb, ipnum) << endl;
		}
	}
}
