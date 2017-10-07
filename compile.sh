#!/bin/bash
# this is to get the c programme compliled to the location
# author        : kneerunjun@gmail.com
# purpose       : only to get the progam compiled and place the executable in the right spot
# after we have discovered problems with the upstarte service we are not using the upstart service. instead we are going ahead with sensing service only
gcc ./device.c -o ./bin/device -lwiringPi -lwiringPiDev -lm -L./adc -ladc -L ./mq135 -lmq135 -L./ldr -lldr
# # gcc ./device-upstart.c -o ./bin/device-upstart -l wiringPi -l wiringPiDev
# sudo systemctl disable co2sensing.service
# sudo systemctl enable co2sensing.service
# # sudo systemctl enable co2sensing.upstart.service
# sudo systemctl daemon-reload
# # sudo systemctl stop co2sensing.upstart.service
# # sudo systemctl stop co2sensing.upstart.service
# sudo systemctl start co2sensing.service
