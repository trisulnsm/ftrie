#include "GeoAPI.h"
#include "GeoDB.h"
#include <string>
#include <tuple>
#include <string.h>

using namespace std;

static const char * get_csv_field(const char * str, char * bufout, size_t bufsize, int tokenid)
{

	const char * pptr = str;
	const char * pptr_end = str;

	int idx=0;
	int inc=0;
	while (*pptr)
	{
		if (*pptr=='"') {
			++pptr;
			pptr_end  = strchrnul(pptr,'"');
			inc=2;
		} else {
			pptr_end  = strchrnul(pptr,',');
			inc=1;
		}
		if( tokenid==idx) {
			memcpy(bufout, pptr, pptr_end-pptr);
			bufout[pptr_end-pptr]=0;
			return bufout;
		}
		pptr = pptr_end+inc;
		++idx;
	}
	return "";
}


GeoIP * 		GeoIP_open(const char * path, uint32_t flags)
{
	using keylabel_t = std::pair<string,string>;					
	using geonames_t= unordered_map<int, keylabel_t >;						// geoid, <key,label>
	using subnet_geoid_t=std::tuple<bool,uint32_t, uint8_t, int>;			// f_valid,netnum,mask,geoid
	using geoid_desc_t  =std::tuple<bool,int, std::string,std::string>;		// f_valid,geoid,key,label

	std::string pathstr(path);
	if (pathstr.find( "GeoLite2-Country-Blocks-IPv4.csv") != string::npos) 
	{
		// country db
		string namesdb(pathstr);
		string toSearch("GeoLite2-Country-Blocks-IPv4.csv");

		// Get the first occurrence
		auto pos = namesdb.find(toSearch);
		namesdb.replace(pos, toSearch.size(), string("GeoLite2-Country-Locations-en.csv"));
			 
		auto pdb = new CGeoDB;

		// load the blocks  cidr->geoid 
		pdb->LoadBlocks( pathstr, 
			[](int line_no, const string& line)->subnet_geoid_t {
				int a,b,c,d,e,geoname_id;
				if (sscanf(line.c_str(),"%d.%d.%d.%d/%d,%d", &a,&b,&c,&d,&e,&geoname_id)==6) {
					uint32_t addr = (a<<24) | (b << 16) | (c << 8)  | d ;
					return std::make_tuple(true, addr, e, geoname_id);
				} else {
					return std::make_tuple(false, 0, 0, 0);
				}
			},
			[](int line_no, const string& line)->geoid_desc_t {
					return std::make_tuple(false, 0, "", "");

			}
		);

		// load the names geoid->key\tdescription
		pdb->LoadNames( namesdb, 
			[](int line_no, const string& line)->subnet_geoid_t {
				return std::make_tuple(false, 0, 0, 0);
			},
			[](int line_no, const string& line)->geoid_desc_t {
					int geoname_id;
					if (sscanf(line.c_str(),"%d", &geoname_id)==1) {
						char buf[256];
						string skey  = get_csv_field(line.c_str(), buf,256, 4);
						string slabel  = get_csv_field(line.c_str(), buf,256, 5);
						if (skey.empty()) {
							// 6255147,en,AS,Asia,,,0 
							skey  = get_csv_field(line.c_str(), buf,256, 2);
							slabel  = get_csv_field(line.c_str(), buf,256, 3);
						}

						return std::make_tuple(true, geoname_id,  skey, slabel );
					} else {
						return std::make_tuple(false, 0,  "", "");
					}
			}
		);

		return (void *) pdb;
	}
	else if (pathstr.find( "GeoLite2-ASN-Blocks-IPv4.csv") != string::npos) 
	{
		// asn db
		auto pdb = new CGeoDB();

		// load the blocks  cidr->geoid 
		pdb->LoadBlocks( pathstr, 
			[](int line_no, const string& line)->subnet_geoid_t {
				int a,b,c,d,e,geoname_id;
				if (sscanf(line.c_str(),"%d.%d.%d.%d/%d,%d", &a,&b,&c,&d,&e,&geoname_id)==6) {
					uint32_t addr = (a<<24) | (b << 16) | (c << 8)  | d ;
					return std::make_tuple(true, addr, e, geoname_id);
				} else {
					return std::make_tuple(false, 0, 0, 0);
				}
			},
			[](int line_no, const string& line)->geoid_desc_t {
				int a,b,c,d,e,geoname_id;
				if (sscanf(line.c_str(),"%d.%d.%d.%d/%d,%d", &a,&b,&c,&d,&e,&geoname_id)==6) {
						char buf[256];
						string skey  = get_csv_field(line.c_str(), buf,256, 1);
						string slabel  = get_csv_field(line.c_str(), buf,256, 2);

						return std::make_tuple(true, geoname_id,  skey, slabel );
				} else {
					return std::make_tuple(false, 0, "", "");
				}
			}
		);

		return pdb;

	}
	else if (pathstr.find( "GeoLite2-Prefix-Blocks-IPv4.csv") != string::npos) 
	{
		// asn db
		auto pdb = new CGeoDB();

		// load the blocks  cidr->geoid 
		pdb->LoadBlocks( pathstr, 
			[](int line_no, const string& line)->subnet_geoid_t {
				int a,b,c,d,e;
				if (sscanf(line.c_str(),"%d.%d.%d.%d/%d", &a,&b,&c,&d,&e)==5) {
					uint32_t addr = (a<<24) | (b << 16) | (c << 8)  | d ;
					return std::make_tuple(true, addr, e, line_no);
				} else {
					return std::make_tuple(false, 0, 0, 0);
				}
			},
			[](int line_no, const string& line)->geoid_desc_t {
				int a,b,c,d,e;
				if (sscanf(line.c_str(),"%d.%d.%d.%d/%d", &a,&b,&c,&d,&e)==5) {
					char buf[256];
					string skey  = get_csv_field(line.c_str(), buf,256, 0);
					return std::make_tuple(true, line_no,  skey, skey);
				} else {
					return std::make_tuple(false, 0, "", "");
				}
			}
		);
		return pdb;
	}
	else if (pathstr.find( "GeoLite2-City-Blocks-IPv4.csv") != string::npos) 
	{
		// city db
		string namesdb(pathstr);
		string toSearch("GeoLite2-City-Blocks-IPv4.csv");

		// Get the first occurrence
		auto pos = namesdb.find(toSearch);
		namesdb.replace(pos, toSearch.size(), string("GeoLite2-City-Locations-en.csv"));
			 
		auto pdb = new CGeoDB;

		// load the blocks  cidr->geoid 
		pdb->LoadBlocks( pathstr, 
			[](int line_no, const string& line)->subnet_geoid_t {
				int a,b,c,d,e,geoname_id;
				if (sscanf(line.c_str(),"%d.%d.%d.%d/%d,%d", &a,&b,&c,&d,&e,&geoname_id)==6) {
					uint32_t addr = (a<<24) | (b << 16) | (c << 8)  | d ;
					return std::make_tuple(true, addr, e, geoname_id);
				} else {
					return std::make_tuple(false, 0, 0, 0);
				}
			},
			[](int line_no, const string& line)->geoid_desc_t {
					return std::make_tuple(false, 0, "", "");

			}
		);

		// load the names geoid->key\tdescription
		pdb->LoadNames( namesdb, 
			[](int line_no, const string& line)->subnet_geoid_t {
				return std::make_tuple(false, 0, 0, 0);
			},
			[](int line_no, const string& line)->geoid_desc_t {
				int geoname_id;
				if (sscanf(line.c_str(),"%d",&geoname_id)==1) {
						char buf[256];
						string scc    = get_csv_field(line.c_str(), buf,256, 4);
						string sst    = get_csv_field(line.c_str(), buf,256, 6);
						string scity  = get_csv_field(line.c_str(), buf,256, 10);
						if (scc.empty()) {
							scc    = get_csv_field(line.c_str(), buf,256, 2);
						}

						return std::make_tuple(true, geoname_id,  to_string(geoname_id), scc +"/"+sst+"/"+scity );
				} else {
					return std::make_tuple(false, 0, "", "");
				}
			}
		);

		pdb->shrink_to_fit();

		return (void *) pdb;
	}
	else 
	{
		return nullptr;
	}
}


void			GeoIP_delete(GeoIP * pdb)
{
	CGeoDB * p = (CGeoDB*) pdb;
	delete p;
}


bool GeoIP_by_ipnum(GeoIP * GeoIP_Handle, uint32_t ipnum, const char ** key, const char ** label)
{
	CGeoDB  * pdb = (CGeoDB *) GeoIP_Handle;
	return pdb->LookupFull(ipnum, key,label);
}


