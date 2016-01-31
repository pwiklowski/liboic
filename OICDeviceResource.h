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

    string getHref() { return m_href;}
    string getResourceType() { return m_resourceType; }
    string getInterface() { return m_interface; }

    void post(cbor *value, COAPResponseHandler handler);
    void get(COAPResponseHandler handler);
    void observe(COAPResponseHandler handler);
private:
    OICDevice* m_device;
    OICClient* m_client;
    string m_href;
    string m_interface;
    string m_resourceType;

};

#endif // OICDEVICERESOURCE_H
