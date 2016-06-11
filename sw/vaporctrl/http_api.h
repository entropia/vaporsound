#pragma once
#include <stdint.h>
#include <microhttpd.h>

#include "buffer.h"

struct endpoint {
	const char *prefix;
	int (*getter)(const char *url, char **out);
	int (*setter)(const char *url, sb_t *data, char **out);
};

struct MHD_Daemon *http_api_start(uint16_t port, struct endpoint **endpoints);
