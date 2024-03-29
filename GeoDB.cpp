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
: nprefixes(0),
  zero_dot_zero(-1)
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
	if (val != 0xffffffff ) {
		geonames_t::iterator it = mpGeonames.find(geonames_t::key_type(val));
		if (it != mpGeonames.end()) {
			const keylabel_t&  kl = it->second;
			*ppkey = kl.first.c_str(); 
			*pplabel = kl.second.c_str();
			return true;
		}
	} else if (zero_dot_zero != -1) {
		geonames_t::iterator it = mpGeonames.find(geonames_t::key_type(zero_dot_zero));
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
	if (val != 0xffffffff ) {
		geonames_t::iterator it = mpGeonames.find(geonames_t::key_type(val));
		if (it != mpGeonames.end()) {
			const keylabel_t&  kl = it->second;
			*ppkey = kl.first.c_str(); 
			*pplabel = kl.second.c_str();
			return true;
		}
	} else if (zero_dot_zero != -1) {
		geonames_t::iterator it = mpGeonames.find(geonames_t::key_type(zero_dot_zero));
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
void 	 CGeoDB::LoadBlocks(const std::string&  csvfile,
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
			if (not b.iszero()) {
				auto iter=b.new_iterator();
				sproot.insert(iter,std::get<3>(nets));
			} else {
				zero_dot_zero=std::get<3>(nets);
			}
			++nprefixes;
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
void			 CGeoDB::LoadNames(   const std::string& csvfile,
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

void			 CGeoDB::Push( int id, uint32_t prefix,  int mask , const std::string& key, const std::string& label )
{
	bits32_t b(prefix, mask ) ;
	if (not b.iszero()) {
		auto iter=b.new_iterator();
		sproot.insert(iter,id);
	} else {
		zero_dot_zero=id;
	}
	++nprefixes;

	mpGeonames.insert(geonames_t::value_type(id, make_pair(key,label)));
}
