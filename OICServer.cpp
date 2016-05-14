#include "OICServer.h"
#include <stdio.h>
#include "log.h"
#include "cbor.h"
#include <stdio.h>
#include "String.h"
#include "COAPObserver.h"


bool OICServer::discoveryRequest(COAPServer* server, COAPPacket* request, COAPPacket* response){
    response->setType(COAP_TYPE_ACK);
    if (request->getCode() == COAP_METHOD_GET){
        response->setResonseCode(COAP_RSPCODE_CONTENT);


        cbor root(CBOR_TYPE_ARRAY);
        cbor device(CBOR_TYPE_MAP);

        device.append("di", m_id);
        device.append("n", m_name);

        cbor links(CBOR_TYPE_ARRAY);

        for(uint16_t i=0; i<m_resources.size();i++){
            cbor val(CBOR_TYPE_MAP);

            val.append("href", m_resources.at(i)->getHref());
            val.append("rt", m_resources.at(i)->getResourceType());
            val.append("if", m_resources.at(i)->getInterface());
            val.append("type", "application/cbor");

            links.append(val);
        }
        device.append("links", links);
        root.append(device);
        root.dump(response->getPayload());

        List<uint8_t> data;
        data.append(((uint16_t)COAP_CONTENTTYPE_CBOR & 0xFF00) >> 8);
        data.append(((uint16_t)COAP_CONTENTTYPE_CBOR & 0xFF));

        response->addOption(new COAPOption(COAP_OPTION_CONTENT_FORMAT, data));
        response->setMessageId(request->getMessageId());

        return true;
    }
    return false;

}

bool OICServer::onRequest(COAPServer* server, COAPPacket* request, COAPPacket* response){
    response->setType(COAP_TYPE_ACK);

    OICResource* resource = getResource(request->getUri());
    if (request->getCode() == COAP_METHOD_GET){
        response->setResonseCode(COAP_RSPCODE_CONTENT);


        if (resource != 0){
            resource->value().dump(response->getPayload());
        }

        List<uint8_t> data;
        data.append(((uint16_t)COAP_CONTENTTYPE_CBOR & 0xFF00) >> 8);
        data.append(((uint16_t)COAP_CONTENTTYPE_CBOR & 0xFF));

        response->addOption(new COAPOption(COAP_OPTION_CONTENT_FORMAT, data));

        return true;

    }else if(request->getCode() == COAP_METHOD_POST){
        response->setResonseCode(COAP_RSPCODE_CHANGED);

        cbor message;
        cbor::parse(&message, request->getPayload());

        if (resource != 0){
            resource->update(message, true);
        }

        return true;

    }
    return false;

}
void OICServer::start(){
    COAPCallback discoveryCallback = [=](COAPServer* server, COAPPacket* request, COAPPacket* response) {
        return this->discoveryRequest(server, request, response);
    };

    coap_server.addResource("/oic/res", discoveryCallback);

    for(uint16_t i=0; i<m_resources.size();i++){
        coap_server.addResource(m_resources.at(i)->getHref(), [=](COAPServer* server, COAPPacket* request, COAPPacket* response) {
            return this->onRequest(server, request, response);
        });
    }
}

OICServer::~OICServer(){
    for(uint16_t i=0; i<m_resources.size(); i++){
        delete m_resources.at(i);
    }
}

OICResource* OICServer::getResource(String href){
    for(uint16_t i=0; i<m_resources.size();i++){
        if (m_resources.at(i)->getHref() == href) return m_resources.at(i);
    }
    return 0;
}

