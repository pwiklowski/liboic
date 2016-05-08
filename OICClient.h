#ifndef OICCLIENT_H
#define OICCLIENT_H

#include "OICBase.h"
#include "OICDevice.h"

class OICClient : public OICBase
{
public:
    OICClient(COAPSend sender);

    void ping(String address, COAPResponseHandler handler);
    void searchDevices(COAPResponseHandler handler);

private:

};

#endif // OICCLIENT_H
