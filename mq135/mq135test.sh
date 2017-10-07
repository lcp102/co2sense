#!/bin/bash
gcc -c mq135.c -I./ -I../adc
ar crv libmq135.a mq135.o ../adc/adc.o
ranlib libmq135.a
gcc mq135test.c -o mq135test -lm -L../adc/ -ladc -L./ -lmq135
./mq135test
