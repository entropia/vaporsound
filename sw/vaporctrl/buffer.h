#pragma once

#include <stdbool.h>
#include <string.h>
#include <sys/types.h>

struct sized_buffer;
typedef struct sized_buffer sb_t;

sb_t *sb_new(size_t size);
int sb_append(sb_t *sb, const char *str, size_t len);
size_t sb_len(sb_t *sb);
char *sb_get(sb_t *sb);
void sb_free(sb_t *sb);
bool sb_overflowed(sb_t *sb);
