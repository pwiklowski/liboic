#ifndef OICCLIENT_H
#define OICCLIENT_H

#include "OICBase.h"
#include "OICDevice.h"

class OICClient : public OICBase
{
public:
    OICClient();

    void searchDevices();

private:
    vector<OICDevice*> m_devices;

};

#endif // OICCLIENT_H
