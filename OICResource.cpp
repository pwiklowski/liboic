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

void OICResource::update(cbor* value) {
    delete m_value;
    m_value = value;
    if (m_onUpdate != 0) m_onUpdate(m_value);
}
