#!/bin/bash
gcc client.c ./generator/gen-c_glib/test.c ./generator/gen-c_glib/generator_types.c ./parser/request_parser.c ./collections/linked_list.c -o client.o \
    -I/usr/lib/x86_64-linux-gnu/glib-2.0/include/ \
    -I/usr/include/glib-2.0 \
    -lglib-2.0 \
    -lgobject-2.0 \
    -lthrift_c_glib \
    -lcypher-parser 
