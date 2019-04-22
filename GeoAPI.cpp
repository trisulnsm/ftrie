#include "GeoAPI.h"
#include "GeoDB.h"
#include <string>
#include <tuple>
#include <string.h>
#include <sstream>
#include <arpa/inet.h>
#include "StrTokenizer.h"

using namespace std;

	static const char * AsciiHex = 
	"\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0"
	"\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0"
	"\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0"
	"\x0\x1\x2\x3\x4\x5\x6\x7\x8\x9\x0\x0\x0\x0\x0\x0"
	"\x0\xa\xb\xc\xd\xe\xf\x0\x0\x0\x0\x0\x0\x0\x0\x0"
	"\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0"
	"\x0\xa\xb\xc\xd\xe\xf\x0\x0\x0\x0\x0\x0\x0\x0\x0"
	"\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0";

	// n1 and n2 are Nibble
	static inline uint8_t    Hex2Byte(char n1, char n2)
	{
		uint8_t b1 =  AsciiHex[ n1&0x7f];
		uint8_t b2 =  AsciiHex[ n2&0x7f];

		return (b1<<4)|b2;
	}
	// Trisul key to ipnum  for v4
	// Trisul key format for v4 : C0.A8.01.02
	static inline uint32_t IP4_to_number(const char  * pszString )
	{
		const char * pptr = pszString;
		union{
			uint8_t  b[4];
			uint32_t dw;
		};

		b[0]=Hex2Byte(pptr[0],pptr[1]);
		b[1]=Hex2Byte(pptr[3],pptr[4]);
		b[2]=Hex2Byte(pptr[6],pptr[7]);
		b[3]=Hex2Byte(pptr[9],pptr[10]);
		return htonl(dw);
	}

static const char * get_csv_field(const char * str, char * bufout, size_t bufsize, int tokenid, char delim=',')
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
			pptr_end  = strchrnul(pptr,delim);
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

		pdb->collapse_leaves();
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
		pdb->collapse_leaves();
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

		pdb->collapse_leaves();

		return (void *) pdb;
	}
	else if (pathstr.find( "aspath") != string::npos) 
	{
		//16.0.1		IN TXT	"34224 3356 174 2519" "1.0.16.0" "24"
		auto pdb = new CGeoDB();

		// load the blocks  cidr->geoid 
		pdb->LoadBlocks( pathstr, 
			[](int line_no, const string& line)->subnet_geoid_t {

				// over * lines 
				if (line.find("*") != string::npos) {
					return std::make_tuple(false, 0, 0, 0);
				}

				int spos=0;
				for (int i=0;i<3;i++) {
					spos= line.find('\t',spos+1);
				}

				string k = line.substr(spos+1);

				char buf[256];
				string ipa      = get_csv_field(k.c_str(), buf,256, 1);
				string cidr     = get_csv_field(k.c_str(), buf,256, 2);

				int a,b,c,d,e;
				if (sscanf(ipa.c_str(),"%d.%d.%d.%d", &a,&b,&c,&d)==4) {
					uint32_t addr = (a<<24) | (b << 16) | (c << 8)  | d ;
					return std::make_tuple(true, addr, stoi(cidr), line_no);
				} else {
					return std::make_tuple(false, 0, 0, 0);
				}
			},
			[](int line_no, const string& line)->geoid_desc_t {

				int spos=0;
				for (int i=0;i<3;i++) {
					spos= line.find('\t',spos+1);
				}

				string k = line.substr(spos+1);

				char buf[256];
				string aspath   = get_csv_field(k.c_str(), buf,256, 0);
				string ipa      = get_csv_field(k.c_str(), buf,256, 1);
				string cidr     = get_csv_field(k.c_str(), buf,256, 2);

				return std::make_tuple(true, line_no,  aspath.append(" ").append(ipa).append("/").append(cidr) , "");
			}
		);
		pdb->collapse_leaves();
		return pdb;
	}
	else if (pathstr.find( "pir-as-path") != string::npos) 
	{
		// 2019-04-22 13:38:16.067 prefix announce 1.11.230.0/24 origin IGP aspath 9498 6939 4766 38091 38091 38091 38091 38091 38091 17839
		auto pdb = new CGeoDB();

		// load the blocks  cidr->geoid 
		pdb->LoadBlocks( pathstr, 
			[](int line_no, const string& line)->subnet_geoid_t {

				// over * lines 
				if (line.find("*") != string::npos) {
					return std::make_tuple(false, 0, 0, 0);
				}

				// must have prefix announcement 
				if (line.find("announce") == string::npos) {
					return std::make_tuple(false, 0, 0, 0);
				}

				StrTokenizer Tok( line.c_str(), " ");
				if (Tok.GetCount() < 9) {
					return std::make_tuple(false, 0, 0, 0);
				}

				int a,b,c,d,e;
				if (sscanf(Tok.GetToken(4) ,"%d.%d.%d.%d/%d", &a,&b,&c,&d,&e)==5) {
					uint32_t addr = (a<<24) | (b << 16) | (c << 8)  | d ;
					return std::make_tuple(true, addr, e, line_no);
				} else {
					return std::make_tuple(false, 0, 0, 0);
				}
			},
			[](int line_no, const string& line)->geoid_desc_t {

				StrTokenizer Tok( line.c_str(), " ");
				if (Tok.GetCount() < 9) {
					return std::make_tuple(false, 0, "", "");
				}

				std::ostringstream oss;
				string ipa      = Tok.GetToken(4);

				for (int i=8; i< Tok.GetCount(); i++) {
					oss << Tok.GetToken(i) << " ";
				}

				oss << Tok.GetToken(4);
				return std::make_tuple(true, line_no,  oss.str()  , "");
			}
		);
		pdb->collapse_leaves();
		return pdb;
	}
	else 
	{
		fprintf(stderr,"database name must contain GeoLite2-City-Blocks/ASN-Blocks/Country-Blocks\n");
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


bool GeoIP_by_key(GeoIP * GeoIP_Handle, const char * ipkey, const char ** key, const char ** label)
{
	return GeoIP_by_ipnum( GeoIP_Handle, IP4_to_number(ipkey), key, label);

}
bool GeoIP_by_ipaddr(GeoIP * GeoIP_Handle, const char * dotted, const char ** key, const char ** label)
{
	CGeoDB  * pdb = (CGeoDB *) GeoIP_Handle;
	return pdb->LookupFull(dotted, key,label);

}
