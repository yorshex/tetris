#!/bin/sh
x86_64-w64-mingw32-gcc -L libwin -I includewin -O3 -otetris src/tetris.c -l:libraylibdll.a -lm
