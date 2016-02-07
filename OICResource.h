#ifndef OICRESOURCE_H
#define OICRESOURCE_H


#include "String.h"
#include "COAPServer.h"
#include <functional>
#include <iostream>
#include "cbor.h"
#include "COAPObserver.h"

using namespace std;


class OICResource
{
public:
    OICResource(String href, String rt, String iff, function<void(cbor *)> onUpdate, cbor *initial);
    ~OICResource();

    void setCoapServer(COAPServer* s){ m_coapServer = s; }

    String getHref(){ return m_href; }
    String getResourceType(){ return m_rt; }
    String getInterface(){ return m_if; }

    COAPCallback getCallback(){ return m_callback;}


    void update(cbor* value, bool notify = false);
    void onUpdate(cbor* value) { m_onUpdate(value);}

    cbor* value() {return m_value;}

    void addObserver(COAPObserver* observer);
private:
    String m_href;
    String m_rt;
    String m_if;
    COAPCallback m_callback;
    COAPServer* m_coapServer;


    cbor* m_value;
    function<void(cbor*)> m_onUpdate;
};

#endif // OICRESOURCE_H
