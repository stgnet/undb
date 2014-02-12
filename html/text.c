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

#include "html.h"

static const char * is_entity(char c)
{
	switch (c)
	{
		case '"': return("&quot;");
		case '\'': return("&apos;");
		case '&': return("&amp;");
		case '<': return("&lt;");
		case '>': return("&gt;");
	}
	return 0;
}

struct html_page text(const char *text)
{
	struct html_page page = PAGE_INIT;

	while (*text)
	{
		const char *start=text;
		while (*text && !is_entity(*text)) text++;
		if (text>start) buffer_append(&page.body, start, text-start);

		if (*text)
			buffer_add_string(&page.body, is_entity(*text++));
	}
	return page;
}
