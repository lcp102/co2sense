#### Detailing of how the device communicates to the cloud
****

A C HTTP REST client would talk to a Django service behind Gunicorn, running on Docker container.
The C HTTP REST client (mostly CURL) can send int GET, PUT, POST, DELETE verbs to public URLs but open without any authentication. We would be fixing the authentication in some upcoming Milestone.
For now the Url would work with no authentication.


For Milestone 1 We need to just get this device online and let it record the measurements. We handle device registration and device deregistrations as per the needs

1. Device boots up
2. Device tries to self register :While the device tries to register itself, it has to send in information as the id (This is hard embedded in the device looping program) , type of the device and the location where the device has been attached to.
  a.Success :
    1. The device is already registered - gets back its own id.
    2. Device not registered : the api would register the device and then send the Id
  c. Failure:
    1. The device can indicate on the LCD of the error in registry and shutdown in 5 minutes
    2. The device can start Co2 measurement but offline - visual indication that the device is offline
3. Device records the air params
4. Device uploads the air params to the cloud
  a. Success : this clears the device to record new params
  b. Failure :
    1. Device indicates the inability to send the data to cloud
    2. Device records the params on the local cache - this is the offline mode
