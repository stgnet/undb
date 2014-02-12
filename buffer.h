/*
    UNDB - Untitled Database Project
    Copyright (C) 2014  Scott Griepentrog

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include <glob.h>

/*
	fast buffer handling
	- stores multiple non-continguous malloc's
	- quickly append to existing available space or add new chunk
	- assumes ability to issue multiple writes to transmit buffer
	- assumes no ability to search for string crossing chunks
	- primarily used for generating html output pages
*/

struct fast_buffer_chunk;

struct fast_buffer_chunk
{
	unsigned char *data;
	size_t size;
	size_t used;
	// available = size - used
	struct fast_buffer_chunk *next;
};

struct fast_buffer
{
	struct fast_buffer_chunk *first_chunk;
	struct fast_buffer_chunk *last_chunk;
};

struct fast_buffer_chunk *chunk_create(size_t size);
/* struct fast_buffer *buffer_create(void); */
void buffer_init(struct fast_buffer *buffer);
unsigned char *buffer_require(struct fast_buffer *buffer, size_t needed);
void buffer_confirm(struct fast_buffer *buffer, size_t added);
void buffer_append(struct fast_buffer *buffer, const unsigned char *buf, size_t len);
void buffer_add_string(struct fast_buffer *buffer, const char *s);
size_t buffer_write(struct fast_buffer *buffer, int fd);
size_t buffer_length(struct fast_buffer *buffer);
void buffer_destroy(struct fast_buffer *buffer);
void buffer_printf(struct fast_buffer *buffer, const char *fmt, ...);
void buffer_append_buffer(struct fast_buffer *buffer, struct fast_buffer *append);
