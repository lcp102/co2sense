#!/bin/bash
# this is to build and run the adctest
gcc -c adc.c
gcc -c adctest.c -I .
gcc -o adctest adctest.o adc.o -lwiringPi
./adctest
