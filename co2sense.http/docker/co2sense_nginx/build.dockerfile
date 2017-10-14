FROM docker_base-ubuntu-14-04:latest
MAINTAINER kneerunjun

ARG SITENAME
ARG PORT

RUN apt-get update
RUN apt-get install -y nginx

RUN cat /etc/nginx/nginx.conf

ADD ./upstart.py /usr/local/bin/nginx.upstart.py
RUN chmod a+x /usr/local/bin/nginx.upstart.py
RUN nginx.upstart.py $SITENAME $PORT

# lets just go aheadto print the conf file once
RUN cat /etc/nginx/nginx.conf
RUN service nginx stop
ENTRYPOINT ["nginx","-g","daemon off;"]
