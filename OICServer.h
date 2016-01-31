#ifndef OICSERVER_H
#define OICSERVER_H

#include "OICBase.h"

class OICServer : public OICBase
{
public:
    OICServer(string name):OICBase(name){}
    void start(string ip, string interface);
    void addResource(OICResource* res) {res->setCoapServer(&coap_server); m_resources.push_back(res);}
private:
    static void* runDiscovery(void*param);
    bool onRequest(COAPServer* server, COAPPacket* request, COAPPacket* response);
    bool discoveryRequest(COAPServer* server, COAPPacket* request, COAPPacket* response);
    OICResource* getResource(string href);
    pthread_t m_discoveryThread;
    vector<OICResource*> m_resources;
};

#endif // OICSERVER_H
