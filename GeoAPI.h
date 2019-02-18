#pragma once

#include <stdint.h>

typedef void	 GeoIP;


GeoIP * 		GeoIP_open(const char * path, uint32_t flags);
void			GeoIP_delete(GeoIP * pdb);
bool 		    GeoIP_by_ipnum(GeoIP * GeoIP_Handle, uint32_t ipnum, const char ** key, const char ** label);
