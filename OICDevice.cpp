#include "OICDevice.h"
#include "OICDeviceResource.h"

OICDevice::OICDevice(string id, string name, string address, OICClient *client)
{
    m_id = id;
    m_name = name;
    m_address = address;
    m_client = client;
}

