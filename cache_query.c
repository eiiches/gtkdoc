/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2010 Eiichi Sato
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#include <glib.h>
#include <string.h>

#include "dh-link.h"
#include "dh-parser.h"

void
make_query_for_cache(const gchar *filename)
{

	GError *error = NULL;
	GNode *tree = NULL;
	GList *keywords = NULL;

	dh_parser_read_file (filename, &tree, &keywords, &error);
	if (error)
		goto catch;

	GRegex *regex = g_regex_new("'", 0, 0, NULL);

	void
	callback(DhLink *link, gpointer user_data)
	{
		const gchar *name = dh_link_get_name(link);
		gchar *escaped = g_regex_replace_literal(regex, name, -1, 0, "''", 0, NULL);
		g_printf("INSERT OR IGNORE INTO tmp VALUES ('%s', '%s');\n", escaped, dh_link_get_uri(link));
		g_free(escaped);
	}
	g_list_foreach(keywords, (GFunc)callback, NULL);

	g_regex_unref(regex);

	goto finally;
catch:
	g_warning("%s: failed to open '%s'", G_STRFUNC, filename);

finally:

	return;
}

gint
main(gint argc, gchar **argv)
{
	gint i;
	for (i = 1; i < argc; ++i)
		make_query_for_cache(argv[i]);
	return 0;
}
