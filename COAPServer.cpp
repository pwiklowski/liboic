#include "COAPServer.h"
#include <cstdio>
#include <string.h>
#include "log.h"

using namespace std;

COAPServer::COAPServer()
{
}



COAPPacket* COAPServer::handleMessage(COAPPacket* p){
    string uri = p->getUri();
    log(uri.c_str());

    COAPPacket* response = new COAPPacket(p->getHeader()->mid, p->getToken());

    auto endpoint = m_callbacks.find(uri);
    if (endpoint != m_callbacks.end()){
        bool success = endpoint->second(p, response);
        if (!success){
            response->setResonseCode(COAP_RSPCODE_FORBIDDEN);
        }
        return response;
    }
    return response;
}


void COAPServer::addResource(string url, COAPCallback callback){
    m_callbacks.insert(make_pair(url, callback));
}



