#include "OICDevice.h"
#include "OICDeviceResource.h"
#include "log.h"

OICDevice::OICDevice(string id, string name, string address, OICClient *client)
{
    log("New device id:'%s' name:'%s' addr:'%s'\n", id.c_str(), name.c_str(), address.c_str());
    m_id = id;
    m_name = name;
    m_address = address;
    m_client = client;
}

void OICDevice::send(COAPPacket* packet, COAPResponseHandler func){
    m_client->send(packet, func);
}
