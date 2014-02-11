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

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "options.h"

struct options options;

static struct option long_options[] =
{
	/* These options set a flag. */
	{"verbose", no_argument,		&options.verbose, 1},
	{"brief",no_argument,		&options.verbose, 0},
	/* These options don't set a flag.
				We distinguish them by their indices. */
	{"add",	no_argument,		0, 'a'},
	{"append",	no_argument,		0, 'b'},
	{"delete",	required_argument, 0, 'd'},
	{"create",	required_argument, 0, 'c'},
	{"file",	required_argument, 0, 'f'},
	{"port",	required_argument, 0, 'p'},
	{0, 0, 0, 0}
};

void options_default(void)
{
	options.verbose=0;
	options.port=80;
}

void options_process(int argc, char **argv)
{
	int c;

	options_default();

	while (1)
	{
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long (argc, argv, "abp:",
						long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;

		switch (c)
		{
		case 0:
			/* If this option set a flag, do nothing else now. */
			if (long_options[option_index].flag != 0)
				break;
			printf ("option %s", long_options[option_index].name);
			if (optarg)
				printf (" with arg %s", optarg);
			printf ("\n");
			break;

		case 'a':
			puts ("option -a\n");
			break;

		case 'b':
			puts ("option -b\n");
			break;

		case 'c':
			printf ("option -c with value `%s'\n", optarg);
			break;

		case 'd':
			printf ("option -d with value `%s'\n", optarg);
			break;

		case 'f':
			printf ("option -f with value `%s'\n", optarg);
			break;

		case 'p':
			options.port = atoi(optarg);
			printf("Port set to %d\n", options.port);
			break;

		case '?':
			/* getopt_long already printed an error message. */
			break;

		default:
			abort ();
		}
	}

	/* Instead of reporting ‘--verbose’
	and ‘--brief’ as they are encountered,
	we report the final status resulting from them. */
	if (options.verbose)
		puts ("verbose flag is set");

	/* Print any remaining command line arguments (not options). */
	if (optind < argc)
	{
		printf ("non-option ARGV-elements: ");
		while (optind < argc)
			printf ("%s ", argv[optind++]);
		putchar ('\n');
	}
}
