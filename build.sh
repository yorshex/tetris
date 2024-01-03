#!/bin/sh

[ -z ${CC} ] && CC=clang
[ -z ${LIBDIR} ] && LIBDIR=./lib
[ -z ${INCDIR} ] && INCDIR=./include
[ -z ${SRCDIR} ] && SRCDIR=./src
[ -z ${BUILDDIR} ] && BUILDDIR=./build

SRC="${SRCDIR}/tetris.c"
CFLAGS="-pedantic -Wall -Wextra -L${LIBDIR} -I${INCDIR} -O3"
LIBS='-l:libraylib.a -lm'

${CC} ${WINSTUFF} ${CFLAGS} -otetris ${SRC} ${LIBS} &&
    echo "\033[1;32mBuilding finished successfully\033[m" ||
    echo "\033[1;31mBuilding finished abnormally\033[m"
