#include "OICClient.h"
#include "OICDevice.h"
#include "OICDeviceResource.h"

OICClient::OICClient():
    OICBase("")
{

}

void OICClient::searchDevices(){
    COAPPacket* p = new COAPPacket();
    p->setType(COAP_TYPE_CON);
    p->setResonseCode(COAP_METHOD_GET);
    p->addOption(new COAPOption(COAP_OPTION_URI_PATH, "oic"));
    p->addOption(new COAPOption(COAP_OPTION_URI_PATH, "res"));

    //p->setAddress("ff02::fd 5683");
    p->setAddress("224.0.1.187 5683");


    send(p->getAddress(), p, [&](COAPPacket* packet){

        cbor* message = cbor::parse(packet->getPayload());

        for (uint16_t i=0; i<message->toArray()->size(); i++){
            cbor* device = message->toArray()->at(i);

            string name = device->getMapValue("n")->toString();
            string di= device->getMapValue("di")->toString();

            cbor* links = device->getMapValue("links");
            OICDevice* dev = new OICDevice(di, name, packet->getAddress(), this);

            for (uint16_t j=0; j< links->toArray()->size(); j++){
                cbor* link = links->toArray()->at(j);


                string href = link->getMapValue("href")->toString();
                string rt = link->getMapValue("rt")->toString();
                string iff = link->getMapValue("if")->toString();

                dev->getResources()->push_back(new OICDeviceResource(href, iff, rt, dev, this));
            }
            m_devices.push_back(dev);
            delete packet;
        }
    });

}
