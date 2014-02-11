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

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdarg.h>
#include <string.h>

#include "httpd.h"

int http_header(int socket, const char *fmt, ...)
{
	char buf[256];

	if (fmt == NULL)
	{
		write(socket,"\x0D\x0A\x0D\x0A",4);
	}
	va_list va;
	va_start(va, fmt);
	vsnprintf(buf, sizeof(buf)-3, fmt, va);
	va_end(va);
	strcat(buf,"\x0D\x0A");
	write(socket, buf, strlen(buf));
}

static char date_buf[40];
char *http_date(void)
{
	time_t now = time(0);
	struct tm tm = *gmtime(&now);
	strftime(date_buf, sizeof(date_buf), "%a, %d %b %Y %H:%M:%S %Z", &tm);
	return(date_buf);
}

int http_error(int socket, int code, char *message)
{
	http_header(socket, "HTTP/1.1 %d %s", code, message);
	http_header(socket, "Date: %s",http_date());
	http_header(socket, "Server: undb");
	http_header(socket, "Connection: close");
	http_header(socket, "Content-Type: text/plain");
	http_header(socket, "Content-Length: 0");
	http_header(socket,NULL);
}

int complete_header(const char *buf, int len)
{
	const char *end = buf + len;
	const char *scan = buf;

	end -= 4;
	while (scan <= end)
	{
		if (scan[0] == 0x0D &&
			scan[1] == 0x0A &&
			scan[2] == 0x0D &&
			scan[3] == 0x0A)
		{
			scan+=4;
			return(scan-buf);
		}
		scan++;
	}
	return 0;
}

void *httpd_connection_handler(void *socket_ptr) {
	char buf[4096];
	int have=0;
	int got=0;
	int socket = *((int *)socket_ptr);
	int header_len;

	while ((got=read(socket, buf+have, sizeof(buf)-have)) >0 )
	{
		have+=got;
		header_len = complete_header(buf, have);
		if (header_len)
			break;
	}
	http_error(socket,500,"Internal Server Error");
	close(socket);
	free(socket_ptr);
	return(0);
}

void httpd(const char *bind_addr, int port)
{
	struct sockaddr_in listen_addr;
	int listen_socket, connection, *thread_socket_ptr;
	pthread_t thread_id;

	listen_socket=socket(AF_INET, SOCK_STREAM, 0);
	if (listen_socket<0)
		exit((perror("Unable to open socket"), 1));

	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = INADDR_ANY;
	listen_addr.sin_port = htons(port);

	if (bind(listen_socket, (struct sockaddr *)&listen_addr, sizeof(listen_addr)) < 0)
		exit((perror("Unable to bind socket"), 2));

	listen(listen_socket, 20);

	while (1)
	{
		connection = accept(listen_socket, NULL, NULL);

		thread_socket_ptr = malloc(sizeof(int));

		*thread_socket_ptr = connection;
		if (pthread_create(&thread_id, NULL, httpd_connection_handler,
			(void *)thread_socket_ptr) != 0)
		{
			perror("Unable to create pthread");
			close(connection);
			free(thread_socket_ptr);
			continue;
		}
	}
}

