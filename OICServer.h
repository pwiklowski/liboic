#ifndef OICSERVER_H
#define OICSERVER_H

#include "pthread.h"
#include <stdint.h>
#include "OICResource.h"
#include "cbor.h"

typedef void(*OICServerSendData)(uint8_t*, uint16_t);

class OICResource;


class OICServer
{
public:
    OICServer();

    void start(string ip, string interface);
    void addResource(OICResource* res) {m_resources.push_back(res);}
private:
    static void* run(void*param);
    static void* runDiscovery(void*param);
    bool onRequest(COAPServer* server, COAPPacket* request, COAPPacket* response);
    bool discoveryRequest(COAPServer* server, COAPPacket* request, COAPPacket* response);

    OICResource* getResource(string href);

    pthread_t m_thread;
    pthread_t m_discoveryThread;

    vector<OICResource*> m_resources;
};

#endif // OICSERVER_H
