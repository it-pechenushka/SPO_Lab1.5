#!/bin/bash
gcc -g server.c ./generator/gen-c_glib/test.c ./generator/gen-c_glib/generator_types.c -o server.o \
    -I/usr/lib/x86_64-linux-gnu/glib-2.0/include/ \
    -I/usr/include/thrift/lib/c_glib \
    -I/usr/include/glib-2.0 \
    -lglib-2.0 \
    -lgobject-2.0 \
    -lthrift_c_glib
