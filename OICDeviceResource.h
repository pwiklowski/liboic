#ifndef OICDEVICERESOURCE_H
#define OICDEVICERESOURCE_H

#include "OICDevice.h"
#include "String.h"
#include "COAPServer.h"
#include "cbor.h"
#include "OICClient.h"

using namespace std;

class OICDevice;

class OICDeviceResource
{
public:
    OICDeviceResource(String href, String interface, String resourceType, OICDevice* device, OICClient* client);

    String getHref() { return m_href;}
    String getResourceType() { return m_resourceType; }
    String getInterface() { return m_interface; }

    void post(cbor *value, COAPResponseHandler handler);
    void get(COAPResponseHandler handler);
    void observe(COAPResponseHandler handler);
    void unobserve(COAPResponseHandler handler);
private:
    OICDevice* m_device;
    OICClient* m_client;
    String m_href;
    String m_interface;
    String m_resourceType;

};

#endif // OICDEVICERESOURCE_H
