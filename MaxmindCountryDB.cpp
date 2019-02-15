#include <cinttypes>
#include <iostream>
#include <fstream>
#include <string.h>
#include <errno.h>
#include <stdexcept>
#include <memory>
#include "ftrie.h"
#include "bitstring.h"
#include "MaxmindCountryDB.h"


CMaxmindCountryDB::CMaxmindCountryDB(const std::string&  blocksfile, const std::string&  metadata)
{
	LoadBlocks(blocksfile);
	LoadMeta(metadata);
}
CMaxmindCountryDB::~CMaxmindCountryDB()
{

}

const char * 	 CMaxmindCountryDB::LookupFull(const char * dotted)
{
	uint32_t val = sproot.longest_match(dotted);
	if (val != 0xfffffffff ) {
		geonames_t::iterator it = mpGeonames.find(geonames_t::key_type(val));
		if (it != mpGeonames.end()) {
			return it->second.c_str();
		}
	} 
	return "";
}

const char * 	 CMaxmindCountryDB::LookupFull(uint32_t  dwip)
{
	bits32_t b(dwip, 32 ) ;
	auto iter=b.new_iterator();
	uint32_t val= sproot.longest_match(iter, 0xffffffff);
	if (val != 0xfffffffff ) {
		geonames_t::iterator it = mpGeonames.find(geonames_t::key_type(val));
		if (it != mpGeonames.end()) {
			return it->second.c_str();
		}
	} 
	return "";
}

// Load the blocks file 
bool 	 CMaxmindCountryDB::LoadBlocks(const std::string&  csvfile)
{
	std::ifstream file(csvfile);
	if (not file.is_open()) {
		char errbuf[256];
		throw std::domain_error(strerror_r(errno,errbuf,256));
	} 

	std::string line;
	while (getline(file, line)) 
	{
		int a,b,c,d,e,geoname_id;
		if (sscanf(line.c_str(),"%d.%d.%d.%d/%d,%d", &a,&b,&c,&d,&e,&geoname_id)==6) {
			uint32_t addr = (a<<24) | (b << 16) | (c << 8)  | d ;

			bits32_t b(addr, e ) ;
			auto iter=b.new_iterator();
			sproot.insert(iter,geoname_id);
		}
		
	}
	file.close();

}


// Load the meta data - the full mapping
bool 	 CMaxmindCountryDB::LoadMeta(const std::string&  csvfile)
{

	std::ifstream file(csvfile);
	if (not file.is_open()) {
		char errbuf[256];
		throw std::domain_error(strerror_r(errno,errbuf,256));
	} 

	std::string line;
	while (getline(file, line)) 
	{
		int geoname_id;
		if (sscanf(line.c_str(),"%d", &geoname_id)==1) {
			mpGeonames.insert(geonames_t::value_type(geoname_id,line));
		}
	}
	file.close();
}
