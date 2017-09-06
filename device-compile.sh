#!/bin/bash
# this is to get the c programme compliled to the location
# author        : kneerunjun@gmail.com
# purpose       : only to get the progam compiled and place the executable in the right spot

gcc ./device.c -o ./bin/device -l wiringPi -l wiringPiDev -l m
# gcc ./device-deamon.c -o ./bin/device-deamon -l wiringPi -l wiringPiDev
