#include <features.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "html.h"

struct html_page one(char *a, char *b)
{
	struct html_page page = PAGE_INIT;

	buffer_add_string(&page.body,a);
	buffer_add_string(&page.body,b);

	return page;
}

void test1(struct html_page *bogus, const char *s)
{
	const char *tag="test1";
	struct html_page page=html_tag("a","href=bogus",one("one","two"));

	buffer_write(&page.body,1);
}

struct html_page html(const char *html)
{
	struct html_page page = PAGE_INIT;

	buffer_add_string(&page.body, html);

	return page;
}

struct html_page html_merge(struct html_page one, struct html_page two)
{
	buffer_append_buffer(&one.head, &two.head);
	buffer_append_buffer(&one.body, &two.body);
	buffer_append_buffer(&one.tail, &two.tail);
	return (one);
}


struct fast_buffer page(const char *title, struct html_page content)
{
	//struct html_page page = PAGE_INIT;
	struct fast_buffer whole_page = { NULL, NULL };

	buffer_add_string(&whole_page,"<!DOCTYPE html>\n");

	// split up the three components to cheat on html_tag
	struct html_page content_head_as_body = {{NULL,NULL},content.head,{NULL,NULL}};
	struct html_page content_body_as_body = {{NULL,NULL},content.body,{NULL,NULL}};
	struct html_page content_tail_as_body = {{NULL,NULL},content.tail,{NULL,NULL}};

	struct html_page html_contents =
		html_tag("html", 0,
			html_merge(
				html_tag("head", 0, 
					html_merge(
						html_tag("title", 0, text(title)),
						content_head_as_body
					)
				),
				html_tag("body", 0,
					html_merge(
						content_body_as_body,
						content_tail_as_body
					)
				)
			)
		);

	buffer_append_buffer(&whole_page, &html_contents.body);

	return (whole_page);
}

void page_test(void)
{
	printf("Testing page generation:\n");

	struct fast_buffer whole_page = page("test page",html_tag("h3", 0, html("Error")));

	buffer_write(&whole_page,1);

}
