#!/bin/bash
# this is to build and run the adctest
gcc -c adc.c -I .
ar crv libadc.a adc.o
ranlib libadc.a
gcc -c adctest.c -I .
gcc -o adctest adctest.o -L. -ladc -lwiringPi
./adctest
