from django.shortcuts import render
from django.http import HttpResponse,Http404,HttpResponseNotFound,HttpResponseBadRequest,HttpResponseNotAllowed,HttpResponseServerError
from django.http import JsonResponse
import json
from django.views.decorators.csrf import csrf_exempt
import pdb, logging
from pymongo import MongoClient
# Create your views here.
# this is just the test data that we are maintaining here - this would be replaced by the actual mongo db
regDevices =[
    {"id":"10D768F4-2871-42B8-96B8-D65F7B98E9C1","type":"Rpi3B", "location":"kneerunjun.home"},
    {"id":"49E7B230-E20D-4C7D-A92D-D5EB91150B4B","type":"Rpi3B+", "location":"kneerunjun.home"},
    {"id":"36E0AE29-252B-4175-B2F7-5BD0AA21BB51","type":"BeagleBone", "location":"kneerunjun.home"},
    {"id":"88AACD6E-DBC9-4724-9958-02C7993D870E","type":"AduinoUno", "location":"kneerunjun.home"},
    {"id":"CF17E115-9990-4DEB-8A3A-99822D360444","type":"Rpi3B", "location":"kneerunjun.home"},
    {"id":"2DA716C4-B577-4F70-B55B-CB2510939DD0","type":"Rpi3B", "location":"kneerunjun.home"},
    {"id":"B4671C68-4ED6-4B4E-9E04-9FE644308DDB","type":"Rpi0w", "location":"kneerunjun.home"},
]
logging.basicConfig(level=logging.DEBUG)
def get_deviceofid(deviceid):
    '''This method needs to be replaced with the database query moving ahead from here.
    For the testing purposes we have connected to the same to a cached data
    deviceid    : id of the device to be found
    '''
    # this method helps in getting the device of a certain id from eother
    try:

        client = MongoClient('mongodb://co2sense_mongo:27017/')
        logging.debug("Hurray we are connected to the database!!")
    except Exception as e:
        logging.warning("failed to connect to the database")
        return None
    result = list(filter(lambda x:x['id']==deviceid, regDevices))
    if len(result) >0:
        return result[0]
    else:
        return None
def parse_body_fromjson(body):
    try:
        return json.loads(body.decode('utf-8'))
    except Exception as e:
        return None
def new_device_ok(devicedetails):
    if devicedetails != None:
        if 'id' in devicedetails and 'type' in devicedetails and 'location' in devicedetails:
            return True
        else:
            return False
    else:
        return False
def register_new_device(devicedetails):
    # this is where we fire the query to the database and create a new device registration
    logging.debug("Registered new device..")
@csrf_exempt
def index(request):
    return JsonResponse({'message':'Hello world ! from inside django app'})
@csrf_exempt
def device_ping(request, deviceid):
    logging.debug("{0}:{1}".format(request.method, request.path_info))
    if request.method =='POST':
        # the body incoming is not expected to be in the unicode format
        payload = parse_body_fromjson(request.body)
        if payload == None:
            logging.warning("failed to parse the request body from json")
            return HttpResponseBadRequest("failed to parse the request body from json")
        # if there is no device by that name we need to revert bacjk with bad request
        if get_deviceofid(deviceid) ==None:
            return HttpResponseBadRequest("Failed to get device with id {0}".format(deviceid))
        return JsonResponse({'message':'From inside the post method',
    'device':deviceid, 'temp':payload['temp'], 'co2':payload['co2'], 'light':payload['light']})
    else :
        return HttpResponseNotAllowed("Device ping : Only POST requests allowed")
@csrf_exempt
def devices(request, deviceid):
    '''This lets you register the devices when not registered with the details
    The method only allows POST method for the method
    '''
    logging.debug("{0}:{1}".format(request.method, request.path_info))
    if request.method  =="POST":
        logging.debug("{0}:{1}".format(request.method, request.path_info))
        if get_deviceofid(deviceid) == None:
            # here we are now ready to register the device
            payload = parse_body_fromjson(request.body)
            if payload == None:
                logging.warning("Body for the request is invlaid")
                return HttpResponseBadRequest("Body for the request invalid")
            if new_device_ok(payload) != True:
                logging.warning("New device being registered is not ok")
                return HttpResponseBadRequest("New device being registered is not ok")
            if payload["id"] != deviceid:
                # this is the case when the url id of the device and the device id are not the same
                logging.warning("Device id not matching")
                return HttpResponseBadRequest("Device id not matching")
            # herewe simply go ahead to register the new device
            logging.debug("New device {0} registered ".format(deviceid))
            register_new_device(payload)
            return JsonResponse({"id":deviceid})
        else:
            # device si already registered
            logging.warning("Device id {0} already registered ".format(deviceid))
            return JsonResponse({"id":deviceid})
    elif request.method =="GET":
        # here the device is just trying to contact server and get its own information
        try:
            device =get_deviceofid(deviceid)
            if device != None:
                # sends back the device information
                return JsonResponse(device)
            else:
                logging.debug("Device ping GET:Device with the id {0} not found".format(deviceid))
                return HttpResponseBadRequest("Device ping GET: Device with the id not found")
        except Exception as e:
            # now i feel we should actually see if we can log the errors to afile
            logging.warning("failed database connection")
            return HttpResponseServerError("Devices Ping GET: Failed query on database")
    else:
        logging.warning("Device tried accessing a method with a verb that is not allowed")
        return HttpResponseNotAllowed("Method verb not allowed")
