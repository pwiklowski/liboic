#include "OICDeviceResource.h"


OICDeviceResource::OICDeviceResource(string href, string interface, string resourceType, OICDevice *device, OICClient *client)
{
    m_device = device;
    m_client = client;
    m_href = href;
}

void OICDeviceResource::post(cbor* value, COAPResponseHandler handler){

    COAPPacket* p = new COAPPacket();
    p->setType(COAP_TYPE_CON);
    p->setResonseCode(COAP_METHOD_POST);
    p->setAddress(m_device->getAddress());

    value->dump(p->getPayload());
    delete value;

    COAPPacket::parseUri(p, m_href);

    vector<uint8_t> data;
    data.push_back(((uint16_t)COAP_CONTENTTYPE_CBOR & 0xFF00) >> 8);
    data.push_back(((uint16_t)COAP_CONTENTTYPE_CBOR & 0xFF));

    p->addOption(new COAPOption(COAP_OPTION_CONTENT_FORMAT, data));

    m_client->send(p->getAddress(), p, handler);
}

void OICDeviceResource::get(COAPResponseHandler handler){
    COAPPacket* p = new COAPPacket();
    p->setType(COAP_TYPE_CON);
    p->setResonseCode(COAP_METHOD_GET);
    p->setAddress(m_device->getAddress());

    COAPPacket::parseUri(p, m_href);

    m_client->send(p->getAddress(), p, handler);


}

void OICDeviceResource::observe(COAPResponseHandler handler){
    COAPPacket* p = new COAPPacket();
    p->setType(COAP_TYPE_CON);
    p->setResonseCode(COAP_METHOD_GET);
    p->setAddress(m_device->getAddress());

    vector<uint8_t> data;
    p->addOption(new COAPOption(COAP_OPTION_OBSERVE, data));

    COAPPacket::parseUri(p, m_href);

    m_client->send(p->getAddress(), p, handler);
}
