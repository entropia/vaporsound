#pragma once
#include <stdint.h>
#include <microhttpd.h>

#include "buffer.h"

typedef int (*http_api_getter)(const char *url, char **out);
typedef int (*http_api_setter)(const char *url, sb_t *data, char **out);

void http_api_register(const char *url, http_api_getter getter, http_api_setter setter);
struct MHD_Daemon *http_api_start(uint16_t port);
