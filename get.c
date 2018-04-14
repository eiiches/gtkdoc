/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2008 Imendio AB
 * Copyright (C) 2008 Sven Herzberg
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
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gprintf.h>

static const gchar *
find_in_buffer (const gchar *buffer,
                const gchar *key,
                gsize        length,
                gsize        key_length)
{
        gsize m = 0;
        gsize i = 0;

        while (i < length) {
                if (key[m] == buffer[i]) {
                        m++;
                        if (m == key_length) {
                                return buffer + i - m + 1;
                        }
                } else {
                        m = 0;
                }
                i++;
        }

        return NULL;
}

static gchar *
get_document(const gchar *uri)
{
	g_return_val_if_fail(uri, NULL);

	const gchar *anchor;
	const gchar *contents;
	GMappedFile *file = NULL;
	gchar *filename = NULL;
	gsize length;
	gchar *key = NULL;
	gsize key_length;
	gsize offset = 0;
	const gchar *start;
	const gchar *end;
	gchar *result = NULL;

	anchor = strrchr(uri, '#');
	if (!anchor)
		goto catch;

	filename = g_strndup(uri, anchor - uri);
	anchor++;

   g_fprintf (stderr, "filename: %s\n", filename);
   g_fprintf (stderr, "anchor: %s\n", anchor);

	if (g_str_has_prefix (filename, "file://"))
		offset = 7;

	file = g_mapped_file_new(filename + offset, FALSE, NULL);
	if (!file)
		goto catch;

	contents = g_mapped_file_get_contents(file);
	length = g_mapped_file_get_length(file);

	key = g_strdup_printf("<a name=\"%s\"", anchor);
	key_length = strlen(key);

	start = find_in_buffer(contents, key, length, key_length);
	if (!start)
		goto catch;

	end = NULL;

	{
		const gchar *startkey;
		const gchar *endkey;

         GError *err = NULL;
         GMatchInfo *matchInfo;
         GRegex *regex;

         regex = g_regex_new ("\\.", 0, 0, &err);
     
         g_regex_match (regex, anchor, 0, &matchInfo);
         if (g_match_info_matches (matchInfo)) {

		length -= start - contents;
		startkey = "<a class=\"link\"";
		start = find_in_buffer(start, startkey, length, strlen(startkey));
		if (start)
		{
   
         regex = g_regex_new ("href=\"(.*?)\"", 0, 0, &err);
     
         g_regex_match (regex, start, 0, &matchInfo);
         if (g_match_info_matches (matchInfo)) {
            gchar *result = g_match_info_fetch (matchInfo, 1);
         
            g_fprintf(stderr, "link=%s\n", result);
            g_fprintf(stderr, "basename=%s\n", g_path_get_dirname(filename));
            filename = g_build_filename(g_path_get_dirname(filename), result, NULL);
            g_fprintf(stderr, "filename=%s\n", filename);
            g_free (result);
            return get_document(filename);
         }
      }
         }

	   startkey = "<pre class=\"programlisting\">";
	   start = find_in_buffer(start, startkey, length, strlen(startkey));
	   if (start)
	   {
	   	endkey = "<div class=\"refsect";
	   	end = find_in_buffer(start, endkey, length - strlen(startkey), strlen(endkey));
	   	if (!end)
	   	{
	   		endkey = "<div class=\"footer";
	   		end = find_in_buffer(start, endkey, length - strlen(startkey), strlen(endkey));
	   	}
	   }
	}

	if (!(start && end))
		goto catch;

	{
		gchar *buf;
		buf = g_strndup(start, end - start);
		result = g_strdup_printf("<div class=\"content\">%s</div>", buf);
		g_free(buf);
	}

	goto finally;
catch:
	g_free(result);
	result = NULL;

finally:
	if (file)
	{
#if GLIB_CHECK_VERSION(2,21,3)
		g_mapped_file_unref(file);
#else
		g_mapped_file_free(file);
#endif
	}
	g_free(filename);
	g_free(key);
	
	return result;
}

gint
main(gint argc, gchar **argv)
{
	gint i, n;
	gint result = EXIT_SUCCESS;

	/* html header */
	g_printf("<html><head></head><body>");

	if (argc <= 1)
	{
		g_printf("<div>No arguments specified.</div>\n");
		result = EXIT_FAILURE;
	}

	for (n = 0, i = 1; i < argc; ++i)
	{
		gchar *doc = get_document(argv[i]);
		if (doc && ++n)
			g_printf("%s", doc);
		g_free(doc);
	}

	if (argc > 1 && n == 0)
	{
		g_printf("<div>No documentation found.</div>\n");
		result = EXIT_FAILURE;
	}

	/* html footer */
	g_printf("</body></html>");

	return result;
}
