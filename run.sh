#!/bin/bash

gcc gol.c -o gol -I ./raylib -L ./raylib -lraylib -lm -lpthread -ldl -lrt -lX11

./gol