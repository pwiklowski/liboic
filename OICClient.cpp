#include "OICClient.h"
#include "OICDevice.h"
#include "OICDeviceResource.h"

OICClient::OICClient():
    OICBase("")
{
    m_is_client = true;
}

void OICClient::searchDevices(COAPResponseHandler handler){
    COAPPacket* p = new COAPPacket();
    p->setType(COAP_TYPE_CON);
    p->setResonseCode(COAP_METHOD_GET);
    p->addOption(new COAPOption(COAP_OPTION_URI_PATH, "oic"));
    p->addOption(new COAPOption(COAP_OPTION_URI_PATH, "res"));

    p->setMessageId(0);
    //p->setAddress("ff02::fd 5683");
    p->setAddress("224.0.1.187 5683");

    send(p, handler);
}
