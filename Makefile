all: build_cache

.PHONY: build_cache
build_cache: get cache_query
	./update_cache

get: get.c
	gcc -o $@ $< `pkg-config --cflags --libs glib-2.0`

cache_query: cache_query.c dh-error.c dh-link.c dh-parser.c dh-error.h dh-link.h dh-parser.h config.h
	gcc -o $@ cache_query.c dh-parser.c dh-link.c dh-error.c `pkg-config --cflags --libs glib-2.0 gobject-2.0` -lz

