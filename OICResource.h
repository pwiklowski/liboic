#ifndef OICRESOURCE_H
#define OICRESOURCE_H


#include <string>
#include "COAPServer.h"

using namespace std;


class OICResource
{
public:
    OICResource(string href, string rt, string iff, COAPCallback callback);
    ~OICResource();


    string getHref(){ return m_href; }
    string getResourceType(){ return m_rt; }
    string getInterface(){ return m_if; }

    COAPCallback getCallback(){ return m_callback;}
private:
    string m_href;
    string m_rt;
    string m_if;
    COAPCallback m_callback;
};

#endif // OICRESOURCE_H
