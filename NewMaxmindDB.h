#pragma once

#include <stdint.h>


extern "C" {

	typedef void	 GeoIP;


	GeoIP * 		GeoIP_open(const char * path, uint32_t flags);
	void			GeoIP_delete(GeoIP * pdb);

	const char *  	GeoIP_country_code_by_ipnum(GeoIP * GeoIP_Handle, uint32_t ipnum);
	const char *    GeoIP_country_name_by_ipnum (GeoIP * GeoIP_Handle, uint32_t ipnum);
	const char *    GeoIP_org_by_ipnum(GeoIP  * GeoIP_Handle, uint32_t ipnum);

}

