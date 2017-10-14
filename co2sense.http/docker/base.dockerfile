FROM ubuntu:14.04
MAINTAINER kneerunjun
RUN apt-get update
RUN apt-get install -y nano htop python3 python3-setuptools
#this  is the base version of the machine over which we would be building nginx , mongo , gunicorn machines 
