#include "OICResource.h"

OICResource::OICResource(string href, string rt, string iff, function<void(cbor *)> onUpdate, cbor* initial)
{
    m_href = href;
    m_rt = rt;
    m_if = iff;

    m_onUpdate = onUpdate;
    m_value = initial;
}

OICResource::~OICResource()
{

}

void OICResource::update(cbor* value, bool notify) {
    //delete m_value;
    m_value = value;

    vector<uint8_t> data;
    m_value->dump(&data);

    m_coapServer->notify(m_href, data);

    if (notify){
        if (m_onUpdate != 0)
            m_onUpdate(m_value);
    }

}
