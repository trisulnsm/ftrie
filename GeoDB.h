#pragma once

#include "ftrie.h"

#include <unordered_map>
#include <functional>
#include <tuple>
using namespace std;

class CGeoDB 
{
public:
	using keylabel_t = std::pair<string,string>;					
	using geonames_t= unordered_map<int, keylabel_t >;						// geoid, <key,label>
	using subnet_geoid_t=std::tuple<bool,uint32_t, uint8_t, int>;			// f_valid,netnum,mask,geoid
	using geoid_desc_t  =std::tuple<bool,int, std::string,std::string>;		// f_valid,geoid,key,label

public:
	CGeoDB();
	virtual ~CGeoDB();

	void			 LoadBlocks( const std::string  &csvfile,
								 std::function<subnet_geoid_t(int, const std::string& line)> f_geoid,
							     std::function<geoid_desc_t(int, const std::string& line)> f_geodesc);

	void			 LoadNames(  const std::string& csvfile,  
								 std::function<subnet_geoid_t(int, const std::string& line)> f_geoid,
							     std::function<geoid_desc_t(int, const std::string& line)> f_geodesc);

	int				 LookupGeoname(const char * dotted);
	int				 LookupGeoname(uint32_t ip);

	bool  			 LookupFull(const char * dotted, const char ** ppkey, const char **pplabel);
	bool		 	 LookupFull(uint32_t  ip, const char **ppkey, const char **pplabel);

	void			collapse_leaves();

	size_t			size() const { return nprefixes;}

	void			 Push( int id, uint32_t prefix,  int mask , const std::string& key, const std::string& label );

private:
	ptreearray_node_4_t  	sproot;
	geonames_t 				mpGeonames;
	size_t					nprefixes=0;
	int						zero_dot_zero=-1;

};

