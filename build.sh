#!/bin/sh
set -xe

CC=gcc
CFLAGS="-Wall -Wextra -pedantic"

$CC $CFLAGS -o ./binview ./binview.c
