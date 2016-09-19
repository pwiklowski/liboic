#include "OICBase.h"
#include <sys/types.h>
#include <stdio.h>

#include "log.h"
#include "cbor.h"
#include <stdio.h>
#include "String.h"
#include "COAPObserver.h"


OICBase::OICBase(String name, COAPSend sender):
    coap_server(sender)
{
    m_name = name;
    m_is_client = false;
}
void OICBase::start(String ip, String interface){

    coap_server.setInterface(interface);
    coap_server.setIp(ip);
}

void OICBase::stop(){

}
void OICBase::sendPacket(COAPPacket* p, COAPResponseHandler handler){
    pthread_mutex_lock(&m_mutex);
    coap_server.sendPacket(p, handler);
    pthread_mutex_unlock(&m_mutex);
}
void OICBase::handleMessage(COAPPacket* p){
    pthread_mutex_lock(&m_mutex);
    coap_server.handleMessage(p);
    pthread_mutex_unlock(&m_mutex);
}

void OICBase::checkPackets(){
    pthread_mutex_lock(&m_mutex);
    coap_server.checkPackets();
    pthread_mutex_unlock(&m_mutex);
}

void OICBase::notify(String href, List<uint8_t> *data){
    pthread_mutex_lock(&m_mutex);
    coap_server.notify(href, data);
    pthread_mutex_unlock(&m_mutex);
}

