#ifndef OICBASE_H
#define OICBASE_H

#include <stdint.h>
#include "OICResource.h"
#include "cbor.h"



typedef void(*OICServerSendData)(uint8_t*, uint16_t);

class OICResource;


class OICBase
{
public:
    OICBase(String name, COAPSend sender);

    void start(String ip, String interface);
    void stop();

    COAPServer* getCoapServer() { return &coap_server; }
    bool isClient() { return m_is_client; }
protected:
    OICResource* getResource(String href);
    uint8_t buffer[1024];
    COAPServer coap_server;
    String m_name;
    bool m_is_client;
};

#endif // OICSERVER_H
