#ifndef COAPServer_H
#define COAPServert_H

#include <string>
#include "COAPPacket.h"


class COAPServer
{
public:
    COAPServer();

    COAPPacket *handleMessage(COAPPacket* p);

private:
};

#endif // COAPPacket_H
