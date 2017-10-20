from django.conf.urls import url
from . import views
urlpatterns = [
    url(r'devices/(?P<deviceid>[A-Z0-9-]{36})/ping/$', views.device_ping, name='device_ping'),
    url(r'devices/(?P<deviceid>[A-Z0-9-]{36})/$', views.devices, name='devices'),
    url(r'$', views.index, name='index'),
    # we are expecting a guid in the format 10D768F4-2871-42B8-96B8-D65F7B98E9C1
]
