all: get cache

.PHONY: get
get:
	gcc -o get get.c `pkg-config --cflags --libs glib-2.0`

.PHONY: cache
cache:
	gcc -o cache_query cache_query.c dh-parser.c dh-link.c dh-error.c `pkg-config --cflags --libs glib-2.0 gobject-2.0` -lz


