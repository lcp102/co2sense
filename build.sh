#!/bin/bash
# this is to get the c programme compliled to the location
# author        : kneerunjun@gmail.com
# purpose       : only to get the progam compiled and place the executable in the right spot
# after we have discovered problems with the upstarte service we are not using the upstart service. instead we are going ahead with sensing service only

# we first compile and build the adc module
gcc -c ./adc/adc.c -I./bin -o ./bin/adc.o
gcc -c ./ldr/ldr.c -I./bin -o./bin/ldr.o
gcc -c ./mq135/mq135.c -I./bin -o./bin/mq135.o
ar crv ./bin/libco2.a ./bin/adc.o ./bin/ldr.o ./bin/mq135.o
ranlib ./bin/libco2.a
gcc ./device.c -o./bin/device -lm -lwiringPi -lwiringPiDev -L ./bin -lco2
