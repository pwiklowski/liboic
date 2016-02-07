#ifndef OICDEVICE_H
#define OICDEVICE_H

#include "String.h"
#include <vector>
#include "COAPPacket.h"
#include "COAPServer.h"


using namespace std;

class OICClient;
class OICDeviceResource;

class OICDevice
{
public:
    OICDevice(String id, String name, String address, OICClient* client);

    vector<OICDeviceResource*>* getResources() { return &m_resources;}

    String getAddress() { return m_address; }

    String getId() { return m_id; }
    String getName() { return m_name;}

    void send(COAPPacket* packet, COAPResponseHandler func);

private:
    String m_id;
    String m_name;
    String m_address;
    OICClient* m_client;
    vector<OICDeviceResource*> m_resources;
};

#endif // OICDEVICE_H
