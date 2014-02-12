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

//#define DEBUG_ENABLED
#ifdef DEBUG_ENABLED
#define DEBUGF(x,...) printf(x,...)
#else
#define DEBUGF(x,...)
#endif


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include "buffer.h" 

#define DEFAULT_CHUNK_SIZE 256

struct fast_buffer_chunk *chunk_create(size_t size)
{
	if (!size) size=DEFAULT_CHUNK_SIZE;
	DEBUGF("chunk_create(%d)\n",size);

	struct fast_buffer_chunk *chunk=malloc(sizeof(struct fast_buffer_chunk)+size);
	chunk->data = ((unsigned char *)chunk)+sizeof(struct fast_buffer_chunk);
	chunk->size = size;
	chunk->used = 0;
	chunk->next = NULL;
	DEBUGF("chunk created %p\n",chunk);
	return(chunk);
}

/* no longer allocating buffer
struct fast_buffer *buffer_create(void)
{
	struct fast_buffer *buffer=malloc(sizeof(struct fast_buffer));
	// should check ptr here, but zero ptr means out of ram,
	// so for now just crash instead

	struct fast_buffer_chunk *chunk=chunk_create(0);
	buffer->first_chunk=chunk;
	buffer->last_chunk=chunk;
}
*/
void buffer_init(struct fast_buffer *buffer)
{
	DEBUGF("buffer_init(%p)\n",buffer);
	struct fast_buffer_chunk *chunk = chunk_create(0);
	buffer->first_chunk=chunk;
	buffer->last_chunk=chunk;
	DEBUGF("after init buffer %p = %p / %p\n",buffer,buffer->first_chunk, buffer->last_chunk);
}

unsigned char *buffer_require(struct fast_buffer *buffer, size_t needed)
{
	DEBUGF("buffer_require(%p, %d)\n",buffer, needed);
	if (!buffer->first_chunk) buffer_init(buffer);
	size_t available = buffer->last_chunk->size - buffer->last_chunk->used;
	if (needed < available)
	{
		unsigned char *ptr=buffer->last_chunk->data + buffer->last_chunk->used;
		return(ptr);
	}
	if (needed < DEFAULT_CHUNK_SIZE) needed = DEFAULT_CHUNK_SIZE;
	struct fast_buffer_chunk *new_chunk = chunk_create(needed);

	buffer->last_chunk->next = new_chunk;
	buffer->last_chunk = new_chunk;

	DEBUGF("after require buffer %p = %p / %p\n",buffer, buffer->first_chunk, buffer->last_chunk);
	unsigned char *ptr = new_chunk->data;
	return(ptr);
}

void buffer_confirm(struct fast_buffer *buffer, size_t added)
{
	DEBUGF("buffer_confirm(%p, %d)\n",buffer, added);

	if (!buffer->first_chunk) buffer_init(buffer);
	size_t available = buffer->last_chunk->size - buffer->last_chunk->used;
	if (added > available)
	{
		printf("FATAL ERROR: confirm_buffer added %d to buffer with %d available\n", added, available);
		exit(99);
	}
	buffer->last_chunk->used += added;
}

void buffer_append(struct fast_buffer *buffer, const unsigned char *buf, size_t len)
{
	DEBUGF("buffer_append(%p, buf, %d)\n", buffer, len);

	if (!buffer->first_chunk) buffer_init(buffer);
	unsigned char *data = buffer_require(buffer, len);
	memcpy(data, buf, len);
	buffer_confirm(buffer, len);
}

void buffer_add_string(struct fast_buffer *buffer, const char *s)
{
	DEBUGF("buffer_add_string(%p, %s)\n",buffer, s);

	if (!buffer->first_chunk) buffer_init(buffer);
	int len = strlen(s);
	unsigned char *data = buffer_require(buffer, len);
	strcpy((char *)data, s);
	buffer_confirm(buffer, len);
}

size_t buffer_write(struct fast_buffer *buffer, int fd)
{
	struct fast_buffer_chunk *chunk;
	size_t total=0;

	if (!buffer->first_chunk)
		return(0);

	chunk = buffer->first_chunk;
	while (chunk)
	{
		DEBUGF("chunk=%p used=%d data=%p\n",chunk, chunk->used, chunk->data);
		if (chunk->used)
		{
			total += chunk->used;
			write(fd, chunk->data, chunk->used);
		}
		chunk = chunk->next;
	}
}

size_t buffer_length(struct fast_buffer *buffer)
{
	struct fast_buffer_chunk *chunk;
	size_t total = 0;

	if (!buffer->first_chunk)
		return(0);

	chunk = buffer->first_chunk;
	while (chunk)
	{
		total += chunk->used;
		chunk = chunk->next;
	}
	return (total);
}

void buffer_destroy(struct fast_buffer *buffer)
{
	struct fast_buffer_chunk *chunk, *next;

	if (!buffer->first_chunk)
		return;

	chunk = buffer->first_chunk;
	while (chunk)
	{
		next = chunk->next;
		free(chunk);
		chunk = next;
	}
	/* buffer may be allocated statically - caller must free it */
}

#define PRINTF_BUFFER_SIZE 128
void buffer_printf(struct fast_buffer *buffer, const char *fmt, ...)
{
	DEBUGF("buffer_printf(%p, %s)\n",buffer, fmt);
	if (!buffer->first_chunk) buffer_init(buffer);
	unsigned char *ptr = buffer_require(buffer, PRINTF_BUFFER_SIZE);
	size_t len;

	va_list va;
	va_start(va, fmt);
	len = vsnprintf(ptr, PRINTF_BUFFER_SIZE-1, fmt, va);
	va_end(va);

	if (len>=PRINTF_BUFFER_SIZE-1)
	{
		printf("WARNING: output buffer truncated due to overflow on %s\n",fmt);
		len=PRINTF_BUFFER_SIZE-1;
		return;
	}

	if (len<0)
	{
		printf("WARNING: output failure on %s\n",fmt);
		return;
	}

	buffer_confirm(buffer, len);
}

void buffer_append_buffer(struct fast_buffer *buffer, struct fast_buffer *append)
{
	DEBUGF("buffer_append_buffer(%p, %p)\n",buffer, append);
	DEBUGF("buffer = %p / %p\n",buffer->first_chunk, buffer->last_chunk);
	DEBUGF("append = %p / %p\n",append->first_chunk, append->last_chunk);

	if (!append->first_chunk) return;
	if (!buffer->first_chunk) buffer_init(buffer);
	buffer->last_chunk->next = append->first_chunk;
	buffer->last_chunk = append->last_chunk;
	append->first_chunk = NULL;
	append->last_chunk = NULL;

	DEBUGF("after append buffer %p = %p / %p\n",buffer,buffer->first_chunk, buffer->last_chunk);
	DEBUGF("after append append %p = %p / %p\n",append,append->first_chunk, append->last_chunk);
	/* buffer may be allocated statically - caller must free it if not */
}

