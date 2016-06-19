#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <libubox/list.h>
#include <microhttpd.h>

#include "buffer.h"
#include "http_api.h"

#define POST_MAX (64*1024)

struct endpoint {
	struct list_head list;

	const char *prefix;
	http_api_getter getter;
	http_api_setter setter;
};

static LIST_HEAD(endpoints);

static void _http_respond(struct MHD_Connection *conn, int ret, char *str, int copy)
{
	struct MHD_Response *resp;
	resp = MHD_create_response_from_buffer(str ? strlen(str) : 0, str, copy);

	MHD_queue_response(conn, ret, resp);
	MHD_destroy_response(resp);
}

static void http_respond_static(struct MHD_Connection *conn, int ret, char *str)
{
	_http_respond(conn, ret, str, MHD_RESPMEM_PERSISTENT);
}

static void http_respond(struct MHD_Connection *conn, int ret, char *str)
{
	_http_respond(conn, ret, str, MHD_RESPMEM_MUST_FREE);
}

static int http_handler(void *cls, struct MHD_Connection *connection, const char *url, const char *method,
		const char *version, const char *upload_data, size_t *upload_data_size, void **con_cls)
{
	(void) cls;
	(void) version;

	int ret;
	char *out = NULL;

	struct endpoint *iter, *ep = NULL;
	list_for_each_entry(iter, &endpoints, list) {
		if(!strncmp(url, iter->prefix, strlen(iter->prefix))) {
			ep = iter;
			break;
		}
	}

	if(!ep) {
		http_respond_static(connection, 404, "endpoint not found");
		return MHD_YES;
	}

	if(!strcmp(method, "GET")) {
		if(!ep->getter) {
			http_respond_static(connection, 400, "get not allowed on endpoint");
			return MHD_YES;
		}

		ret = ep->getter(url, &out);
		http_respond(connection, ret, out);

		return MHD_YES;
	} else if(!strcmp(method, "POST")) {
		if(*con_cls == NULL) { // first time around
			sb_t *sb = sb_new(POST_MAX);
			if(!sb) {
				http_respond_static(connection, 500, NULL);
				return MHD_YES;
			}

			*con_cls = sb;

			return MHD_YES;
		}

		sb_t *sb = *con_cls;

		if(*upload_data_size) {
			// in the middle of getting post data
			if(sb_append(sb, upload_data, *upload_data_size) < 0) {
				// discard the data. otherwise, libmicrohttpd is unhappy.
				*upload_data_size = 0;

				return MHD_YES;
			} else {
				*upload_data_size = 0;
				return MHD_YES;
			}
		} else {
			// all post data received

			if(sb_overflowed(sb))
				http_respond_static(connection, 400, "request too big");
			else {
				if(!ep->setter) {
					http_respond_static(connection, 400, "set not allowed on endpoint");
					return MHD_YES;
				}

				ret = ep->setter(url, sb, &out);
				http_respond(connection, ret, out);
			}

			sb_free(sb);
			return MHD_YES;
		}
	} else {
		http_respond_static(connection, 405, "method not permitted");
		return MHD_YES;
	}
}

void http_api_register(const char *prefix, http_api_getter getter, http_api_setter setter)
{
	struct endpoint *ep = malloc(sizeof(*ep));

	ep->prefix = prefix;
	ep->getter = getter;
	ep->setter = setter;

	list_add_tail(&ep->list, &endpoints);
}

struct MHD_Daemon *http_api_start(uint16_t port)
{
	return MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION | MHD_USE_DEBUG, port,
			NULL, NULL,
			http_handler, NULL,
			MHD_OPTION_END);
}
