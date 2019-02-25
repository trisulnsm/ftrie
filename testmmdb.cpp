#include <errno.h>
#include <maxminddb.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <chrono>
#include <random>
using namespace std;

void	runbenchmark(MMDB_s * pdb);

int main(int argc, char **argv)
{
    char *filename = argv[1];

    MMDB_s mmdb;
    int status = MMDB_open(filename, MMDB_MODE_MMAP, &mmdb);

    if (MMDB_SUCCESS != status) {
        fprintf(stderr, "\n  Can't open %s - %s\n",
                filename, MMDB_strerror(status));

        if (MMDB_IO_ERROR == status) {
            fprintf(stderr, "    IO error: %s\n", strerror(errno));
        }
        exit(1);
    }

	runbenchmark(&mmdb);


	MMDB_close(&mmdb);
	exit(1);
}

void	runbenchmark(MMDB_s * mmdb)
{

	// 10 M random lookup performance
	std::random_device r;
	std::default_random_engine e1(r());
	std::uniform_int_distribution<uint32_t> uniform_dist(0, 0xFFFFFFFF);
	auto start = chrono::steady_clock::now();
	int ok=0,fail=0;
	sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = 0;
	for (size_t i =0; i < 10000000; i ++) 
	{
		uint32_t ipnum  = uniform_dist(e1);

		int err;
		sa.sin_addr.s_addr = ipnum;
		MMDB_lookup_result_s result = MMDB_lookup_sockaddr(mmdb, (sockaddr*)&sa, &err);
		if (result.found_entry)
		{
			MMDB_entry_data_list_s *edl = 0;
			int status = MMDB_get_entry_data_list(&result.entry, &edl);
			if (status == MMDB_SUCCESS)
			{
				++ok;
			}
			else
			{
				++fail;
			}
			MMDB_free_entry_data_list(edl);
		}
	}
	auto end  = chrono::steady_clock::now();
	auto ms   = chrono::duration <double, milli> (end-start).count() ;

	cout << "Finished 10M lookups in " << ms << " milliseconds" << "  at rate " << (double) 1e+10/(ms/1000) << " per second" << endl;
	cout << "lookup ok = " << ok << " , notfound = " << fail << endl;

}
