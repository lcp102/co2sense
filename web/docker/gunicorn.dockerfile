FROM ubuntu:14.04
MAINTAINER kneerunjun
RUN apt-get update
RUN apt-get install -y python3 python3-setuptools
RUN easy_install3 pip
RUN pip3 install django pymongo gunicorn django-cors-headers
ADD ./gunicorn.upstart.py /usr/local/bin/gunicorn.upstart.py
RUN chmod a+x /usr/local/bin/gunicorn.upstart.py
ENTRYPOINT ["gunicorn.upstart.py"]
