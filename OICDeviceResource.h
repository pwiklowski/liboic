#ifndef OICDEVICERESOURCE_H
#define OICDEVICERESOURCE_H

#include "OICDevice.h"
#include <string>
#include "COAPServer.h"
#include "cbor.h"
#include "OICClient.h"

using namespace std;

class OICDevice;

class OICDeviceResource
{
public:
    OICDeviceResource(string href, string interface, string resourceType, OICDevice* device, OICClient* client);


    void post(cbor *value, COAPResponseHandler handler);
    void get(COAPResponseHandler handler);
    void observe(COAPResponseHandler handler);
private:
    OICDevice* m_device;
    OICClient* m_client;
    string m_href;
};

#endif // OICDEVICERESOURCE_H
