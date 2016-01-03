#include "COAPServer.h"
#include <cstdio>
#include <string.h>

using namespace std;

COAPServer::COAPServer()
{
    printf("%s \n", "af");
}



COAPPacket* COAPServer::handleMessage(COAPPacket* p){


    string uri = p->getUri();
    printf("handleRequest %s\n", uri.c_str());

    if (uri.compare("/.well-known/core") == 0){
        string desc;

        for( auto i= m_descriptions.begin(); i!=m_descriptions.end(); i++){
            desc += i->second + "\n";
        }

        vector<uint8_t> data;
        data.push_back(((uint16_t)COAP_CONTENTTYPE_APPLICATION_LINKFORMAT & 0xFF00) >> 8);
        data.push_back(((uint16_t)COAP_CONTENTTYPE_APPLICATION_LINKFORMAT & 0xFF));

        return new COAPPacket(new COAPOption(COAP_OPTION_CONTENT_FORMAT, data), desc, p->getHeader()->id[0], p->getHeader()->id[1], p->getToken(), COAP_RSPCODE_CONTENT);
    }

    auto endpoint = m_endpoints.find(uri);
    if (endpoint != m_endpoints.end()){
        COAPPacket* response = new COAPPacket(p->getHeader()->id[0], p->getHeader()->id[1], p->getToken());
        endpoint->second(p, response);
        return response;
    }
    return new COAPPacket(NULL,"", p->getHeader()->id[0], p->getHeader()->id[1], p->getToken(), COAP_RSPCODE_NOT_FOUND);
}


void COAPServer::addEndpoint(string url, string description, COAPEndpointCallback callback){
    m_endpoints[url] = callback;
    m_descriptions[url] = description;
}


