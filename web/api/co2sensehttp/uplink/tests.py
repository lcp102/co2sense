from django.test import TestCase
from django.test import Client
import json
from pymongo import MongoClient
# Create your tests here.
class UplinkTestCase(TestCase):
    def __init__(self, *args, **kwargs):
        super(UplinkTestCase, self).__init__(*args, **kwargs)
        self.devices =[
            {"id":"10D768F4-2871-42B8-96B8-D65F7B98E9C1","type":"Rpi3B", "location":"kneerunjun.home"},
            {"id":"49E7B230-E20D-4C7D-A92D-D5EB91150B4B","type":"Rpi3B+", "location":"kneerunjun.home"},
            {"id":"36E0AE29-252B-4175-B2F7-5BD0AA21BB51","type":"BeagleBone", "location":"kneerunjun.home"},
            {"id":"88AACD6E-DBC9-4724-9958-02C7993D870E","type":"AduinoUno", "location":"kneerunjun.home"},
            {"id":"CF17E115-9990-4DEB-8A3A-99822D360444","type":"Rpi3B", "location":"kneerunjun.home"},
            {"id":"2DA716C4-B577-4F70-B55B-CB2510939DD0","type":"Rpi3B", "location":"kneerunjun.home"},
            {"id":"B4671C68-4ED6-4B4E-9E04-9FE644308DDB","type":"Rpi0w", "location":"kneerunjun.home"},
        ]
        self.client = MongoClient('mongodb://co2sense_mongo:27017/')
        self.db=self.client["co2sense"]
        self.devices_coll = self.db["devices"]
    def setUp(self):
        self.devices_coll.insert_many(self.devices)
        print("We have {0} devices registred".format(self.devices_coll.count()))
    def tearDown(self):
        all_ids = list(map(lambda x:x["id"], self.devices))
        result =self.devices_coll.delete_many({'id':{'$in':all_ids}})
    def test_uplink_ping(self):
         testDevice = "B4671C68-4ED6-4B4E-9E04-9FE644308DDB"
         badtestDevice = "B4671C68-4ED6-4B4E-9E04-9FE644308AAA"
         c = Client()
        #  this we are assuming is going to be the payload  from the device
         payload ={"temp":28.02, "co2":441.023, "light":35.01}
         response = c.post('/api/uplink/devices/{0}/ping/'.format(testDevice),
         content_type='application/json',data=json.dumps(payload))
         self.assertEqual(response.status_code, 200, "Expecting a 200OK did not get")

         response = c.post('/api/uplink/devices/{0}/ping/'.format(badtestDevice),
         content_type='application/json',data=json.dumps(payload))
         self.assertEqual(response.status_code, 400, "Expecting bad request 400 for non existent device ")

        #we now go ahead and change the verb on the test api
         response = c.put('/api/uplink/devices/{0}/ping/'.format(testDevice),
         content_type='application/json',data=json.dumps(payload))
         self.assertEqual(response.status_code, 405, "Expecting a Not allowed exception on the request")

        #  lets try the get request to see if we can actually get the device intended
    def test_devices_register(self):
         testDevice = {"id":"023BB55B-BD0D-407F-940C-2F777BB0B530","type":"Rpi3B", "location":"kneerunjun.home"}
         badtestDevice = {"id":"10D768F4-2871-42B8-96B8-D65F7B98E9C1","type":"Rpi3B", "location":"kneerunjun.home"}
         c = Client()
         #  this test denotes the happy path when the device being created has not been created earlier.
         # all the device information params are also valid,
         url = '/api/uplink/devices/023BB55B-BD0D-407F-940C-2F777BB0B530/'
         response=c.post(url, data=json.dumps(testDevice), content_type='application/json')
         self.assertEqual(response.status_code, 200, "Expected 200 for new device creation")
         deviceid = json.loads(response.content.decode('utf-8'))
         self.assertEqual(testDevice["id"], deviceid["id"], "the id of the device created and responded are not identical")
         #  next we go ahead to test for the device that has already been registered
         url = '/api/uplink/devices/10D768F4-2871-42B8-96B8-D65F7B98E9C1/'
         response=c.post(url, data=json.dumps(badtestDevice), content_type='application/json')
         deviceid = json.loads(response.content.decode('utf-8'))
         self.assertEqual(response.status_code, 200, "Expected 200 for new device creation")
         self.assertEqual(badtestDevice["id"], deviceid["id"], "the id of the device created and responded are not identical")

         response = c.get('/api/uplink/devices/{0}/'.format(badtestDevice['id']))
         self.assertEqual(response.status_code, 200, "Expecting a 200 ok for the get request")
         dvcInfo  = json.loads(response.content.decode('utf-8'))
         self.assertIsNotNone(dvcInfo, "We were expecting a non null return for the get request")
         print(dvcInfo)

        #  response = c.get('/api/uplink/devices/{0}/'.format(testDevice['id']))
        #  self.assertEqual(response.status_code, 400, "Expecting a 400 device not found")
