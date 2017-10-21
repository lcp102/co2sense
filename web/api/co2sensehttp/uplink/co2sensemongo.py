from pymongo import MongoClient
import logging
def connect_mongo_coll(collName):
    '''This is to connect to the mongodb and the collection as needed
    collName        :the name of the collection you need to connect to
    raises an exception if not able to connect to database
    '''
    try:
        client = MongoClient('mongodb://co2sense_mongo:27017/')
        db=client["co2sense"]
        coll = db[collName]
        return client, coll
    except Exception as e:
        raise e
    finally:
        if client != None:
            client.close()
def get_deviceofid(deviceid):
    '''This method needs to be replaced with the database query moving ahead from here.
    For the testing purposes we have connected to the same to a cached data
    deviceid    : id of the device to be found
    '''
    try:
        client,coll  = connect_mongo_coll("devices")
        device  = coll.find_one({'id':deviceid})
        return device
    except Exception as e:
        raise e
    finally:
        client.close() # closing the connection to the dbase
def register_new_device(devicedetails):
    '''This adds a new device with the details to the database
    devicedetails   : ex :{"id":"10D768F4-2871-42B8-96B8-D65F7B98E9C1","type":"Rpi3B", "location":"kneerunjun.home"}
    '''
    client, coll =connect_mongo_coll("devices")
    if client != None and coll != None:
        try:
            result =coll.insert_one(devicedetails)
            return result.inserted_id
        except Exception as e:
            raise e
        finally:
            client.close()
