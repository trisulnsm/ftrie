#include <iostream>
#include "MaxmindASNDB.h"

using namespace std;

void runtest(CMaxmindASNDB& db);

int main(int c, char ** v)
{
	if (c != 2 ) {
		throw std::domain_error("Usage : testasn   input-csv-blocks ");
	}

	CMaxmindASNDB db( v[1]);

	runtest(db);
}

void runtest(CMaxmindASNDB& db)
{
	vector<string>  testips{
		"49.206.114.59",
		"192.168.3.255",
		"192.168.3.1",
		"192.168.3.0",
		"192.168.2.255",
		"192.168.212.82", 
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
		cout << "Matching IP " << ip << "  value = " 
				<< db.LookupFull(ip.c_str()) << endl;
	}
}
