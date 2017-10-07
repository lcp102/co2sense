#!/bin/bash
gcc -c mq135.c -I./ -I../adc
ar crv ../bin/libmq135.a mq135.o ../adc/adc.o
ranlib ../bin/libmq135.a
gcc mq135test.c -o mq135test -lm -L../bin/ -ladc -lmq135
./mq135test
