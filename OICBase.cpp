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
