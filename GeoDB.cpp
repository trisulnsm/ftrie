#include <cinttypes>
#include <iostream>
#include <fstream>
#include <string.h>
#include <errno.h>
#include <stdexcept>
#include <memory>
#include "ftrie.h"
#include "bitstring.h"
#include "GeoDB.h"


CGeoDB::CGeoDB()
{
}
CGeoDB::~CGeoDB()
{

}

void			CGeoDB::collapse_leaves()
{
	sproot.collapse_leaves();
}

int				 CGeoDB::LookupGeoname(const char * dotted)
{
	return sproot.longest_match(dotted);
}

int				 CGeoDB::LookupGeoname(uint32_t dwip)
{
	bits32_t b(dwip, 32 ) ;
	auto iter=b.new_iterator();
	return sproot.longest_match(iter, 0xffffffff);
}

bool 	CGeoDB::LookupFull(const char * dotted, const char **ppkey, const char **pplabel)
{
	uint32_t val = sproot.longest_match(dotted);
	if (val != 0xfffffffff ) {
		geonames_t::iterator it = mpGeonames.find(geonames_t::key_type(val));
		if (it != mpGeonames.end()) {
			const keylabel_t&  kl = it->second;
			*ppkey = kl.first.c_str(); 
			*pplabel = kl.second.c_str();
			return true;
		}
	} 
	return false;
}

bool 	CGeoDB::LookupFull(uint32_t dwip,  const char **ppkey, const char **pplabel)
{
	bits32_t b(dwip, 32 ) ;
	auto iter=b.new_iterator();
	uint32_t val= sproot.longest_match(iter, 0xffffffff);
	if (val != 0xfffffffff ) {
		geonames_t::iterator it = mpGeonames.find(geonames_t::key_type(val));
		if (it != mpGeonames.end()) {
			const keylabel_t&  kl = it->second;
			*ppkey = kl.first.c_str(); 
			*pplabel = kl.second.c_str();
			return true;
		}
	} 
	return false;
}

// Load the blocks file 
bool 	 CGeoDB::LoadBlocks(const std::string&  csvfile,
							std::function<subnet_geoid_t(int, const std::string& line)> f_geoid,
							std::function<geoid_desc_t(int, const std::string& line)> f_geodesc)
{
	std::ifstream file(csvfile);
	if (not file.is_open()) {
		char errbuf[256];
		throw std::domain_error(strerror_r(errno,errbuf,256));
	} 

	std::string line;
	int line_no=1;
	while (getline(file, line)) 
	{

		auto  nets = f_geoid( line_no, line);
		if (std::get<0>(nets)) {
			bits32_t b(std::get<1>(nets), std::get<2>(nets) ) ;
			auto iter=b.new_iterator();
			sproot.insert(iter,std::get<3>(nets));
		}

		auto names =  f_geodesc( line_no, line);
		if (std::get<0>(names)) {
			mpGeonames.insert(geonames_t::value_type(std::get<1>(names),make_pair(get<2>(names),get<3>(names))));
		}

		++line_no;
	}
	file.close();

}

// Load the meta data - the full mapping
bool			 CGeoDB::LoadNames(   const std::string& csvfile,
									 std::function<subnet_geoid_t(int, const std::string& line)> f_geoid,
							     	 std::function<geoid_desc_t(int, const std::string& line)> f_geodesc)
{
	std::ifstream file(csvfile);
	if (not file.is_open()) {
		char errbuf[256];
		throw std::domain_error(strerror_r(errno,errbuf,256));
	} 

	std::string line;
	int line_no=1;
	while (getline(file, line)) 
	{
		auto names =  f_geodesc( line_no, line);
		if (std::get<0>(names)) {
			mpGeonames.insert(geonames_t::value_type(std::get<1>(names),make_pair(get<2>(names),get<3>(names))));
		}
		++line_no;
	}
	file.close();
}
