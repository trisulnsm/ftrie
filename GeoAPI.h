#pragma once

#include <stdint.h>

typedef void	 GeoIP;

extern "C" {
GeoIP * 		GeoIP_open(const char * path, uint32_t flags);
void			GeoIP_delete(GeoIP * pdb);
bool 		    GeoIP_by_ipnum(GeoIP * GeoIP_Handle, uint32_t ipnum, const char ** key, const char ** label);
bool 		    GeoIP_by_key(GeoIP * GeoIP_Handle, const char * ipkey, const char ** key, const char ** label);
bool 		    GeoIP_by_ipaddr(GeoIP * GeoIP_Handle, const char * dotted, const char ** key, const char ** label);

GeoIP * 		GeoIP_new();
GeoIP * 		GeoIP_load(GeoIP *, const char * path, uint32_t flags);
bool 		    GeoIP_push_v4(GeoIP * GeoIP_Handle, uint32_t netnum, uint8_t mask, const char * key, const char *label);
bool 		    GeoIP_push_v4_str(GeoIP * GeoIP_Handle, const char * prefix, const char * key, const char *label);
int				GeoIP_size(GeoIP * GeoIP_Handle);
}

