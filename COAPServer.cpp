#include "COAPServer.h"
#include <cstdio>
#include <string.h>
#include "log.h"

using namespace std;

COAPServer::COAPServer()
{
    log("sdf");
}



COAPPacket* COAPServer::handleMessage(COAPPacket* p){
    string uri = p->getUri();
    log(uri.c_str());

    COAPPacket* response = new COAPPacket(p->getHeader()->mid, p->getToken());

    if (uri.compare("/.well-known/core") == 0){
        string desc;

        for( auto i= m_descriptions.begin(); i!=m_descriptions.end(); i++){
            desc += i->second + "\n";
        }

        vector<uint8_t> data;
        data.push_back(((uint16_t)COAP_CONTENTTYPE_APPLICATION_LINKFORMAT & 0xFF00) >> 8);
        data.push_back(((uint16_t)COAP_CONTENTTYPE_APPLICATION_LINKFORMAT & 0xFF));

        response->addOption(new COAPOption(COAP_OPTION_CONTENT_FORMAT, data));
        response->addPayload(desc);
        response->setResonseCode(COAP_RSPCODE_CONTENT);

        return response;
    }

    auto endpoint = m_endpoints.find(uri);
    if (endpoint != m_endpoints.end()){
        COAPPacket* response = new COAPPacket(p->getHeader()->mid, p->getToken());
        bool success = endpoint->second(p, response);
        if (!success){
            response->setResonseCode(COAP_RSPCODE_FORBIDDEN);
        }
        return response;
    }

    return response;
}


void COAPServer::addEndpoint(string url, string description, COAPEndpointCallback callback){
    m_endpoints[url] = callback;
    m_descriptions[url] = description;
}


