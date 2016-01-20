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

    COAPPacket* response = new COAPPacket();
    response->setMessageId(p->getHeader()->mid);

    vector<uint8_t> t = p->getToken();

    if (t.size() == 2){
        uint16_t token = t.at(1) << 8 | t.at(0);
        response->setToken(token);
    }


    auto endpoint = m_callbacks.find(uri);
    if (endpoint != m_callbacks.end()){
        bool success = endpoint->second(this, p, response);
        if (!success){
            response->setResonseCode(COAP_RSPCODE_FORBIDDEN);
        }
        return response;
    }else{
        response->setResonseCode(COAP_RSPCODE_NOT_FOUND);
    }
    return response;
}


void COAPServer::addResource(string url, COAPCallback callback){
    m_callbacks.insert(make_pair(url, callback));
}

void COAPServer::addObserver(COAPObserver* observer){
    m_observers.push_back(observer);
}
