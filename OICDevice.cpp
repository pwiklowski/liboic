#include "OICDevice.h"
#include "OICDeviceResource.h"

OICDevice::OICDevice(String id, String name, String address, OICClient *client)
{
    m_id = id;
    m_name = name;
    m_address = address;
    m_client = client;
}

void OICDevice::send_packet(COAPPacket* packet, COAPResponseHandler func){
    m_client->sendPacket(packet, func);
}
