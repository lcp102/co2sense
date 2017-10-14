FROM ubuntu:14.04
MAINTAINER kneerunjun
ARG SITENAME
ARG PORT

RUN apt-get update
RUN apt-get install -y nano nginx python3

ADD ./nginx.upstart.py /usr/local/bin/nginx.upstart.py
RUN chmod a+x /usr/local/bin/nginx.upstart.py
RUN nginx.upstart.py $SITENAME $PORT

RUN service nginx stop
ENTRYPOINT ["nginx","-g","daemon off;"]
