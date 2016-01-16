#include "OICResource.h"

OICResource::OICResource(string href, string rt, string iff, COAPCallback callback)
{
    m_href = href;
    m_rt = rt;
    m_if = iff;
    m_callback = callback;
}

OICResource::~OICResource()
{

}

