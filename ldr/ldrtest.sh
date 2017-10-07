#!/bin/bash
gcc -c ldr.c -I. -I..
ar crv ../bin/libldr.a ldr.o ../adc/adc.o
ranlib libldr.a
gcc ldrtest.c  -o ldrtest -L../bin/ -L../adc/ -ladc -lldr
./ldrtest
