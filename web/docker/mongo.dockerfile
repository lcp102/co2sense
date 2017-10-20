
FROM ubuntu:14.04
MAINTAINER kneerunjun
RUN apt-get update
RUN apt-get install -y python3 python3-setuptools
RUN easy_install3 pip
RUN apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv 7F0CEB10
RUN echo "deb http://repo.mongodb.org/apt/ubuntu "$(lsb_release -sc)"/mongodb-org/3.0 multiverse" | sudo tee /etc/apt/sources.list.d/mongodb-org-3.0.list
RUN apt-get update
RUN apt-get install -y mongodb-org
ADD ./mongo.upstart.py /usr/local/bin/mongo.upstart.py
RUN chmod a+x /usr/local/bin/mongo.upstart.py
# RUN service stop mongodb
# make sure you are sending the commands as in double quotes
ENTRYPOINT ["mongo.upstart.py"]
