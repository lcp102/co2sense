#!/bin/bash
gcc -c ldr.c -I. -I..
ar crv libldr.a ldr.o ../adc/adc.o
ranlib libldr.a
gcc ldrtest.c  -o ldrtest -L. -L../adc/ -ladc -lldr
./ldrtest
