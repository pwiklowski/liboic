#include "COAPServer.h"
#include <cstdio>
#include <string.h>
#include "log.h"
#include "COAPObserver.h"


using namespace std;

COAPServer::COAPServer(COAPSend sender)
{
    m_sender = sender;


    vector<uint8_t> t;
    t.push_back(1);
    t.push_back(4);

}



void COAPServer::handleMessage(COAPPacket* p){

    if(p->getHeader()->t == COAP_TYPE_ACK)
    {
        uint16_t messageId = p->getHeader()->mid;

        auto endpoint = m_responseHandlers.find(messageId);
        if (endpoint != m_responseHandlers.end()){
            endpoint->second(p);

            COAPOption* observeOption = p->getOption(COAP_OPTION_OBSERVE);

            if (observeOption !=0){
                COAPObserver* obs = new COAPObserver(p->getAddress(), p->getUri(), p->getToken(), endpoint->second);
                m_observers.push_back(obs);
            }
            log("Remove response handler %d\n", messageId);
            m_responseHandlers.erase(endpoint);
        }
    }
    else if (p->getHeader()->t == COAP_TYPE_CON)
    {
        string uri = p->getUri();
        log("handle request %s\n", uri.c_str());

        COAPPacket* response = new COAPPacket();
        COAPOption* observeOption = p->getOption(COAP_OPTION_OBSERVE);

        response->setAddress(p->getAddress());
        response->setMessageId(p->getHeader()->mid);

        vector<uint8_t> t = p->getToken();

        if (t.size() == 2){
            uint16_t token = t.at(1) << 8 | t.at(0);
            response->setToken(token);
        }

        if (observeOption)
        {
            uint8_t observe = 0;
            if (observeOption->getData()->size() > 0)
                observe = observeOption->getData()->at(0);

            if (observe == 0){
                COAPObserver* obs = new COAPObserver(p->getAddress(), p->getUri(), p->getToken());
                m_observers.push_back(obs);

                vector<uint8_t> d;
                d.push_back(obs->getNumber());

                response->addOption(new COAPOption(COAP_OPTION_OBSERVE, d));
            }
            else if (observe == 1){
                log("TODO remove observers");

            }
            else{
                for(COAPObserver* o: m_observers) {
                    if (o->getToken() == p->getToken()){
                        o->handle(p);
                        log("notify from server received %s %s\n", o->getAddress().c_str(), o->getHref().c_str());
                        m_sender(response, 0);
                        return;
                    }
                }
                response->setResonseCode(COAP_RSPCODE_NOT_FOUND);
                m_sender(response, 0);
                return;
            }
        }

        auto endpoint = m_callbacks.find(uri);
        if (endpoint != m_callbacks.end()){
            bool success = endpoint->second(this, p, response);
            if (!success){
                response->setResonseCode(COAP_RSPCODE_FORBIDDEN);
            }
        }else{
            response->setResonseCode(COAP_RSPCODE_NOT_FOUND);
        }

        m_sender(response, 0);
    }
}


void COAPServer::addResource(string url, COAPCallback callback){
    m_callbacks.insert(make_pair(url, callback));
}

void COAPServer::addResponseHandler(uint16_t messageId, COAPResponseHandler handler){
    m_responseHandlers.insert(make_pair(messageId, handler));
}

void COAPServer::notify(string href, vector<uint8_t> data){
    for(uint16_t i=0; i<m_observers.size(); i++){
        COAPObserver* o = m_observers.at(i);



        if (href == o->getHref()){
            COAPPacket* p = new COAPPacket();
            p->setType(COAP_TYPE_CON);
            p->setAddress(o->getAddress());
            p->setResonseCode(COAP_RSPCODE_CONTENT);
            p->setToken(o->getToken());

            vector<uint8_t> obs;
            obs.push_back(o->getNumber());
            p->addOption(new COAPOption(COAP_OPTION_OBSERVE, obs));

            vector<uint8_t> part;
            for(uint16_t j=1; j<href.size();j++){
                if (href.at(j) == '/'){
                    p->addOption(new COAPOption(COAP_OPTION_URI_PATH, part));
                    part.clear();
                }else{
                    part.push_back(href.at(j));
                }
            }

            vector<uint8_t> content_type;
            content_type.push_back(((uint16_t)COAP_CONTENTTYPE_CBOR & 0xFF00) >> 8);
            content_type.push_back(((uint16_t)COAP_CONTENTTYPE_CBOR & 0xFF));

            p->addOption(new COAPOption(COAP_OPTION_CONTENT_FORMAT, content_type));
            p->addPayload(data);
            m_sender(p, 0);
        }
    }
}



