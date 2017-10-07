#!/bin/bash
# this is to build and run the adctest
gcc -c adc.c -I .
ar crv ../bin/libadc.a adc.o
ranlib ../bin/libadc.a
gcc -c adctest.c -I .
gcc -o adctest adctest.o -L../bin -ladc -lwiringPi
./adctest
