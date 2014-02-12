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

struct html_page html_tag(const char *name, const char *attributes, struct html_page content)
{
	struct html_page page = PAGE_INIT;

	if (content.head.first_chunk)
		buffer_append_buffer(&page.head, &content.head);
	if (content.tail.first_chunk)
		buffer_append_buffer(&page.tail, &content.tail);

	if (!content.body.first_chunk && !html_tag_dont_self_close(name))
	{
		if (attributes)
			buffer_printf(&page.body,"<%s %s />", name, attributes);
		else
			buffer_printf(&page.body,"<%s />", name);
		return (page);
	}

	if (attributes)
		buffer_printf(&page.body, "<%s %s>", name, attributes);
	else
		buffer_printf(&page.body, "<%s>", name);

	buffer_append_buffer(&page.body,&content.body);

	buffer_printf(&page.body,"</%s>\n", name);

	return(page);
}
