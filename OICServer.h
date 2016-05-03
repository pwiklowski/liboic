#ifndef OICSERVER_H
#define OICSERVER_H

#include "OICBase.h"

class OICServer : public OICBase
{
public:
    OICServer(String name, String di, COAPSend sender):OICBase(name, sender){
        m_id = di;
    }
    void start();
    void addResource(OICResource* res) {res->setCoapServer(&coap_server); m_resources.append(res);}
private:
    static void* runDiscovery(void*param);
    bool onRequest(COAPServer* server, COAPPacket* request, COAPPacket* response);
    bool discoveryRequest(COAPServer* server, COAPPacket* request, COAPPacket* response);
    OICResource* getResource(String href);
    List<OICResource*> m_resources;
    String m_id;
};

#endif // OICSERVER_H
