#!/bin/sh

[ -z ${CC} ] && CC=clang
[ -z ${LIBDIR} ] && LIBDIR=./lib
[ -z ${INCDIR} ] && INCDIR=./include
[ -z ${SRCDIR} ] && SRCDIR=./src
[ -z ${BUILDDIR} ] && BUILDDIR=./build

SRC="${SRCDIR}/main.c"
CFLAGS="-pedantic -Wall -Wextra -L${LIBDIR} -I${INCDIR}"
LIBS='-l:libraylib.a -lm'

${CC} ${CFLAGS} -o${BUILDDIR}/tetris ${SRC} ${LIBS}
