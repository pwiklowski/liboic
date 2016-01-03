#ifndef COAPServer_H
#define COAPServert_H

#include <string>
#include "COAPPacket.h"
#include <map>

typedef void (*COAPEndpointCallback)(COAPPacket*, COAPPacket*);

class COAPServer
{
public:
    COAPServer();

    COAPPacket *handleMessage(COAPPacket* p);
    void addEndpoint(string url, string description, COAPEndpointCallback callback);

private:
    map<string, COAPEndpointCallback> m_endpoints;
    map<string, string> m_descriptions;

};

#endif // COAPPacket_H
