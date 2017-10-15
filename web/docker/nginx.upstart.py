#! /usr/bin/python3
import os , subprocess , time, re, sys
# below is the basic configuration that we need to start nginx with
site_name=sys.argv[1] #this should be able to get us the name of the site
listening_port=sys.argv[2]
cmd ="useradd nginx"
subprocess.call(cmd.split())
config="""
upstream api_server {
    server  co2sense_gunicorn:8000 fail_timeout=0;
}
server {
    #this is the specific port on which we are making the ecolight website accessible
	listen {0} ;
	root /var/www/{1};
	# Make site accessible from http://localhost/
	server_name localhost;
    index index.html;
	location / {
        autoindex on;
	}
    location /libs {
        alias   /var/www/libs;
        autoindex   on;
    }
    location /api{
        include proxy_params;
        proxy_pass  http://api_server;
    }
}
"""
config =re.sub(r"{[0]}", listening_port, config)
config =re.sub(r"{[1]}", site_name, config)

dir_sites_available="/etc/nginx/sites-available"
dir_sites_enabled="/etc/nginx/sites-enabled"
cmd ="touch {0}/{1}".format(dir_sites_available,site_name)
subprocess.call(cmd.split())
with open("{0}/{1}".format(dir_sites_available,site_name), "w") as f:
    f.write(config)
# now is the time when we enable the site
target="{0}/{1}".format(dir_sites_available,site_name)
link_name=site_name
os.chdir(dir_sites_enabled)
cmd ="ln -s {0} {1}".format(target,link_name)
subprocess.call(cmd.split())

old_conf ="/etc/nginx/nginx.conf"
new_conf="/etc/nginx/newnginx.conf"
with open(old_conf, "r") as f:
    lines=f.readlines()
    with open(new_conf, "w") as new_f:
        for l in lines:
            new_f.write(re.sub(r'user [a-z-]*;', "user nginx;",l))
            # this shall change the user for which nginx is operating
os.remove("/etc/nginx/nginx.conf")
os.rename(new_conf,old_conf)
