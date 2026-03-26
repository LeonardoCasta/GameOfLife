#!/bin/bash

gcc gol.c -o gol -I ../raylib/src -L ../raylib/src -lraylib -lm -lpthread -ldl -lrt -lX11

./gol