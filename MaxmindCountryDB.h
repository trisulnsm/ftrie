#pragma once

#include "ftrie.h"
#include <map>
using namespace std;

class CMaxmindCountryDB
{
	using geonames_t= map<int, std::string>;

public:
	CMaxmindCountryDB(const std::string&  blocksfile, const std::string&  metadata);
	virtual ~CMaxmindCountryDB();

	const char * 	 LookupFull(const char * dotted);
	const char * 	 LookupFull(uint32_t  ip);

private:
	bool 	 LoadBlocks(const std::string&  blocksfile);
	bool 	 LoadMeta(const std::string&  metadata);

private:
	ptreearray_node_4_t  	sproot;
	geonames_t 				mpGeonames;

};

