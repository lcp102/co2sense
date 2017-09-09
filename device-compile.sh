#!/bin/bash
# this is to get the c programme compliled to the location
# author        : kneerunjun@gmail.com
# purpose       : only to get the progam compiled and place the executable in the right spot

gcc ./device.c -o ./bin/device -l wiringPi -l wiringPiDev -l m
gcc ./device-upstart.c -o ./bin/device-upstart -l wiringPi -l wiringPiDev
sudo systemctl disable co2sensing.service
sudo systemctl enable co2sensing.upstart.service
sudo systemctl daemon-reload
sudo systemctl stop co2sensing.upstart.service
sudo systemctl start co2sensing.upstart.service
