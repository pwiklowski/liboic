#include "OICResource.h"
#include "COAPServer.h"
#include "log.h"

OICResource::OICResource(String name, String href, String rt, String iff,  ssvu::FastFunc<void(cbor)> onUpdate, cbor *initial)
{
    m_href = href;
    m_rt = rt;
    m_if = iff;
    m_name = name;

    m_onUpdate = onUpdate;
    m_value = initial;
}

OICResource::~OICResource()
{

}

void OICResource::update(cbor value, bool notify) {
    if (notify){
        if (m_onUpdate != nullptr)
            m_onUpdate(value);
        Vector<uint8_t> data;
        (*m_value).dump(&data);

        m_coapServer->notify(m_href, &data);
    }
}
