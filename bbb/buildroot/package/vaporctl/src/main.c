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

	volume_init();

	struct MHD_Daemon *httpd = http_api_start(HTTP_PORT);
	if(!httpd) {
		return EXIT_FAILURE;
	}

	while(1)
		pause();

	MHD_stop_daemon(httpd);

	return EXIT_SUCCESS;
}
