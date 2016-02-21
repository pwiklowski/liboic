#include "OICBase.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>

#ifdef ESP8266
#else
#include <arpa/inet.h>
#include <net/if.h>
#endif
#include <iostream>
#include "log.h"
#include "cbor.h"
#include <netdb.h>
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
#ifdef IPV6
String OICBase::convertAddress(sockaddr_in6 addr){
    String port = to_String(htons(addr.sin6_port));
    char a[30];

    if (inet_ntop(AF_INET6, (sockaddr_in6*)&addr.sin6_addr, a, 30) == 0){
        log("Unable to bind");
        return 0;
    }

    String address(a);
    address += ' ';
    address += port;
    return address;
}
#endif


#ifdef IPV4
String OICBase::convertAddress(sockaddr_in a){
    char addr[30];
    sprintf(addr, "%d.%d.%d.%d %d",
            (uint8_t) (a.sin_addr.s_addr),
            (uint8_t) (a.sin_addr.s_addr >> 8),
            (uint8_t) (a.sin_addr.s_addr >> 16 ),
            (uint8_t) (a.sin_addr.s_addr >> 24),
            htons(a.sin_port));

    return addr;
}
#endif

uint16_t OICBase::getMessageId(){
    m_id++;
    if (m_id == 0) m_id = 1;
    return m_id;
}
