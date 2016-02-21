#ifndef OICBASE_H
#define OICBASE_H

#include "pthread.h"
#include <stdint.h>
#include "OICResource.h"
#include "cbor.h"

#ifdef ESP8266
#include "lwip/sockets.h"
#else
#include <netinet/in.h>
#endif

#define IPV4



typedef void(*OICServerSendData)(uint8_t*, uint16_t);

class OICResource;


class OICBase
{
public:
    OICBase(String name, COAPSend sender);

    void start(String ip, String interface);
    void stop();

#ifdef IPV4
    String convertAddress(sockaddr_in addr);
#endif
    COAPServer* getCoapServer() { return &coap_server; }

    uint16_t getMessageId();
    COAPSend send_packet;
    bool isClient() { return m_is_client; }
protected:
#ifdef IPV6
    String convertAddress(sockaddr_in6 addr);
#endif


    OICResource* getResource(String href);



    uint8_t buffer[1024];
    COAPServer coap_server;
    String m_name;
    bool m_is_client;
    uint16_t m_id;
};

#endif // OICSERVER_H
