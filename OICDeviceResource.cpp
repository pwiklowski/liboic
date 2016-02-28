#include "OICDeviceResource.h"


OICDeviceResource::OICDeviceResource(String href, String interface, String resourceType, OICDevice *device, OICClient *client)
{
    m_device = device;
    m_client = client;
    m_href = href;
    m_interface = interface;
    m_resourceType = resourceType;
}

void OICDeviceResource::post(cbor* value, COAPResponseHandler handler){

    COAPPacket* p = new COAPPacket();
    p->setType(COAP_TYPE_CON);
    p->setResonseCode(COAP_METHOD_POST);
    p->setAddress(m_device->getAddress());

    value->dump(p->getPayload());
    delete value;

    COAPPacket::parseUri(p, m_href);

    List<uint8_t> data;
    data.append(((uint16_t)COAP_CONTENTTYPE_CBOR & 0xFF00) >> 8);
    data.append(((uint16_t)COAP_CONTENTTYPE_CBOR & 0xFF));

    p->addOption(new COAPOption(COAP_OPTION_CONTENT_FORMAT, data));

    m_client->getCoapServer()->sendPacket(p, handler);
}

void OICDeviceResource::get(COAPResponseHandler handler){
    COAPPacket* p = new COAPPacket();
    p->setType(COAP_TYPE_CON);
    p->setResonseCode(COAP_METHOD_GET);
    p->setAddress(m_device->getAddress());

    COAPPacket::parseUri(p, m_href);
    m_client->getCoapServer()->sendPacket(p, handler);
}

void OICDeviceResource::observe(COAPResponseHandler handler){
    COAPPacket* p = new COAPPacket();
    p->setType(COAP_TYPE_CON);
    p->setResonseCode(COAP_METHOD_GET);
    p->setAddress(m_device->getAddress());

    List<uint8_t> data;
    p->addOption(new COAPOption(COAP_OPTION_OBSERVE, data));

    COAPPacket::parseUri(p, m_href);

    m_client->getCoapServer()->sendPacket(p, handler);
}
