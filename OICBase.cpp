#include "OICBase.h"
#include <sys/socket.h>
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
    m_id = 1;
}
void OICBase::start(String ip, String interface){

    coap_server.setInterface(interface);
    coap_server.setIp(ip);
}

void OICBase::stop(){

}
uint16_t OICBase::getMessageId(){
    m_id++;
    if (m_id == 0) m_id = 1;
    return m_id;
}
