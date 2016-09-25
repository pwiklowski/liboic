#ifndef OICBASE_H
#define OICBASE_H

#include <stdint.h>
#include "OICResource.h"
#include "cbor.h"
#include <pthread.h>


typedef void(*OICServerSendData)(uint8_t*, uint16_t);

class OICResource;


class OICBase
{
public:
    OICBase(String name, COAPSend sender);

    void start(String ip, String interface);
    void stop();

    void sendPacket(COAPPacket* p, COAPResponseHandler handler);
    void handleMessage(COAPPacket* p);
    void checkPackets();
    void notify(String href, List<uint8_t> *data);

    bool isClient() { return m_is_client; }
protected:
    OICResource* getResource(String href);
    uint8_t buffer[1024];
    COAPServer coap_server;
    String m_name;
    bool m_is_client;

#ifndef ESP8266
    pthread_mutex_t m_mutex;
#endif
};

#endif // OICSERVER_H
