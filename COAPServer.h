#ifndef COAPServer_H
#define COAPServert_H

#include <string>
#include "COAPPacket.h"
#include <vector>
#include <map>

typedef bool (*COAPCallback)(COAPPacket*, COAPPacket*);


class COAPServer
{
public:
    COAPServer();

    COAPPacket *handleMessage(COAPPacket* p);
    void addResource(string url, COAPCallback callback);

private:

    map<string, COAPCallback> m_callbacks;
};

#endif // COAPPacket_H
