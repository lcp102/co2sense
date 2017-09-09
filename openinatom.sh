#!/bin/bash
# author:kneerunjun@gmail.com
# date: 09 SEP 2017
# this was a shell script just to open the entire folder and requisite files in eatom on the local machine

echo "opening code files in atom, please keep atom remote server running .."
ratom device.c
ratom device-upstart.c
sudo ratom /etc/systemd/system/co2sensing.service
sudo ratom /etc/systemd/system/co2sensing.upstart.service
