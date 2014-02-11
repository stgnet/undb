#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "options.h"
#include "httpd.h"

extern struct options options;

int
main (int argc, char **argv)
{
	options_process(argc, argv);

	printf("Starting HTTPD on port %d\n",options.port);
	httpd(NULL,options.port);
}
