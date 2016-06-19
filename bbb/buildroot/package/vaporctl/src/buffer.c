#include <stdlib.h>

#include "buffer.h"

struct sized_buffer {
	char *buf;
	size_t size;
	off_t off;

	bool overflowed;
};

sb_t *sb_new(size_t size)
{
	sb_t *sb = malloc(sizeof(*sb));

	if(!sb)
		return NULL;

	sb->buf = malloc(size);
	if(!sb->buf) {
		free(sb);

		return NULL;
	}

	sb->size = size;
	sb->off = 0;
	sb->overflowed = false;

	return sb;
}

int sb_append(sb_t *sb, const char *str, size_t len)
{
	size_t avail = sb->size - sb->off;

	if(len >= avail) {
		sb->overflowed = true;
		return -1;
	}

	memcpy(&sb->buf[sb->off], str, len);

	sb->off += len;

	return 0;
}

size_t sb_len(sb_t *sb)
{
	return sb->off;
}

char *sb_get(sb_t *sb)
{
	return sb->buf;
}

void sb_free(sb_t *sb)
{
	free(sb->buf);
	free(sb);
}

bool sb_overflowed(sb_t *sb)
{
	return sb->overflowed;
}
