#include "NewMaxmindDB.h"
#include "MaxmindASNDB.h"
#include "MaxmindCountryDB.h"
#include <string>
#include <re2/re2.h>
using namespace std;


GeoIP * 		GeoIP_open(const char * path, uint32_t flags)
{

	std::string pathstr(path);
	if (pathstr.find( "Country-Blocks-IPv4.csv") != string::npos) 
	{
		// country db
		std::string  namesdb(path);
		RE2::GlobalReplace(&namesdb, "GeoLite2-Country-Blocks-IPv4.csv", "GeoLite2-Country-Locations-en.csv");

		return (void *) new CMaxmindCountryDB ( pathstr, namesdb);
	}
	else if (pathstr.find( "ASN-Blocks-IPv4.csv") != string::npos) 
	{
		// asn db
		return (void *) new CMaxmindASNDB( pathstr);
	}
	else 
	{
		return nullptr;
	}
}


void			GeoIP_delete(GeoIP * pdb)
{
	CMDB * p = (CMDB*) pdb;
	delete p;
}


const char *  	GeoIP_country_code_by_ipnum(GeoIP * GeoIP_Handle, uint32_t ipnum)
{
	CMaxmindCountryDB  * pdb = (CMaxmindCountryDB *) GeoIP_Handle;
	return pdb->LookupFull(ipnum);
}


const char *    GeoIP_country_name_by_ipnum (GeoIP * GeoIP_Handle, uint32_t ipnum)
{
	CMaxmindCountryDB  * pdb = (CMaxmindCountryDB *) GeoIP_Handle;
	return pdb->LookupFull(ipnum);
}


const char *    GeoIP_org_by_ipnum(GeoIP * GeoIP_Handle, uint32_t ipnum)
{
	CMaxmindASNDB  * pdb = (CMaxmindASNDB *) GeoIP_Handle;
	return pdb->LookupFull(ipnum);


}