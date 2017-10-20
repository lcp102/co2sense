#! /usr/bin/python3
import os , subprocess , time, re, sys
# NOTE: Since the /data/db path is not established or there is no directory the mongodb service would not be alive ata lla

dataDir ="/var/db/co2sense"
logDir ="/var/log/co2sense"
command = "chown -R mongodb {0}".format(dataDir)
command2 = "chown -R mongodb {0}".format(logDir)
command3 = "chmod -R 777 {0}".format(dataDir)
command4 = "chmod -R 777 {0}".format(logDir)
try:
    subprocess.call(command.split())
    subprocess.call(command2.split())
    subprocess.call(command3.split())
except Exception as e:
    print("failed to change the ownership and opmode of data directory..")
    sys.exit(1)
startCommand  ="mongod --dbpath {0} --logpath {1}/dbase.log".format(dataDir, logDir)
try:
    subprocess.call(startCommand.split())
except Exception as e:
    print("Failed to start database..")
    print(str(e))
    sys.exit(1)
