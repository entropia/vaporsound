#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "config.h"
#include "http_api.h"
#include "volume.h"

int main(int argc, char **argv)
{
	(void) argc;
	(void) argv;

	struct endpoint *endpoints[] = {
		volume_init(),
		NULL
	};

	struct MHD_Daemon *httpd;

	httpd = http_api_start(HTTP_PORT, endpoints);
	if(!httpd) {
		return EXIT_FAILURE;
	}

	while(1)
		pause();

	MHD_stop_daemon(httpd);

	return EXIT_SUCCESS;
}
