devhelp for CUI
================

## Requirements:
	- glib 2.10.0 (or greater)
	- sqlite3
	- realpath
	- dirname
	- w3m
	
## Installation:
	Just `make` and done. There's no 'install' target.

## Usage:
	1. Execute `./update_cache` to update index of installed gtk-docs. 
	  Normally this would take several seconds.
	2. `./gtkdoc <keyword>` to open documentation for <keyword>

## Example:

    ./gtkdoc gtk_main_quit

