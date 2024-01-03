#!/bin/sh
CC=x86_64-w64-mingw32-gcc \
LIBDIR=./libwin \
INCDIR=./includewin \
LIBS=-l:libraylibdll.a
./build.sh
