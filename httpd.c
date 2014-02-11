#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "httpd.h"

void *httpd_connection_handler(void *socket_ptr) {
	char buf[1024];
	int have=0;
	int got=0;
	int socket = *((int *)socket_ptr);

	while ((got=read(socket, buf+have, sizeof(buf)-have)) >0 )
	{
		have+=got;
	}
	write(socket,"Thank you\n",10);
	write(1,buf,have);

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

