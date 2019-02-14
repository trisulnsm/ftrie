#pragma once

#include "ftrie.h"
#include <map>
using namespace std;

class CMaxmindASNDB
{
	using geonames_t= map<int, std::string>;

public:
	CMaxmindASNDB(const std::string&  blocksfile);
	virtual ~CMaxmindASNDB();

	const char * 	 LookupFull(const char * dotted);
	const char * 	 LookupFull(uint32_t  ip);

private:
	bool 	 		 LoadBlocks(const std::string&  metadata);

private:
	ptreearray_node_4_t  	sproot;
	geonames_t 				mpGeonames;

};

