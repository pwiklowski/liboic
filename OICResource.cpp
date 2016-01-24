#include "OICResource.h"

OICResource::OICResource(string href, string rt, string iff, function<void(cbor *)> onUpdate)
{
    m_href = href;
    m_rt = rt;
    m_if = iff;

    m_onUpdate = onUpdate;
}

OICResource::~OICResource()
{

}

void OICResource::update(cbor* value, bool notify) {
    delete m_value;
    m_value = value;

    for(uint16_t i;i<m_observers.size();i++)
    {
        m_observers.at(i)->notify();
    }

    if (notify){
        if (m_onUpdate != 0)
            m_onUpdate(m_value);
    }

}

void OICResource::addObserver(COAPObserver* observer){
    m_observers.push_back(observer);
}
