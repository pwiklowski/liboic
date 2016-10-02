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
#ifndef ESP8266
    m_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif
}
void OICBase::start(String ip, String interface){

    coap_server.setInterface(interface);
    coap_server.setIp(ip);
}

void OICBase::stop(){

}
void OICBase::sendPacket(COAPPacket* p, COAPResponseHandler handler){
#ifndef ESP8266

    if (pthread_mutex_trylock(&m_mutex) != 0)
    {
        coap_server.queuePacket(p, handler);
        return;
    }
#endif
    coap_server.sendPacket(p, handler);
#ifndef ESP8266
    pthread_mutex_unlock(&m_mutex);
#endif
}
void OICBase::handleMessage(COAPPacket* p){
#ifndef ESP8266
    pthread_mutex_lock(&m_mutex);
#endif
    coap_server.handleMessage(p);
#ifndef ESP8266
    pthread_mutex_unlock(&m_mutex);
#endif
}

void OICBase::sendQueuedPackets(){
#ifndef ESP8266
    pthread_mutex_lock(&m_mutex);
#endif
    coap_server.sendQueuedPackets();
#ifndef ESP8266
    pthread_mutex_unlock(&m_mutex);
#endif


}

void OICBase::checkPackets(){
#ifndef ESP8266
    pthread_mutex_lock(&m_mutex);
#endif
    coap_server.checkPackets();
#ifndef ESP8266
    pthread_mutex_unlock(&m_mutex);
#endif
}

void OICBase::notify(String href, List<uint8_t> *data){
#ifndef ESP8266
    pthread_mutex_lock(&m_mutex);
#endif
    coap_server.notify(href, data);
#ifndef ESP8266
    pthread_mutex_unlock(&m_mutex);
#endif
}

