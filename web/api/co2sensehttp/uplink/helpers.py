
import json
def json_friendly(mongoobject):
    """This method is a gateway safeguard for methods wanting to convert pymongo objects to json safe
    Objectid needs to be removed
    """
    result =dict(mongoobject) #making a copy of the dictionary before removing the key
    del result['_id']
    return result
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
