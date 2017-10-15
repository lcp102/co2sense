#! /usr/bin/python3
import os , subprocess , time, re

os.chdir('/var/http/co2sense/')
command="gunicorn -w 2 -b :8000 co2sensehttp.wsgi"
subprocess.call(command.split())
