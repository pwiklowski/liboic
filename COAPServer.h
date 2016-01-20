#ifndef COAPServer_H
#define COAPServert_H

#include <string>
#include "COAPPacket.h"
#include <vector>
#include <map>
#include "COAPObserver.h"
#include <functional>
#include <iostream>
class COAPServer;

typedef std::function<bool(COAPServer*, COAPPacket*, COAPPacket*)> COAPCallback;



class COAPServer
{
public:
    COAPServer();

    COAPPacket *handleMessage(COAPPacket* p);
    void addResource(string url, COAPCallback callback);


    void setIp(string ip){ m_ip = ip; }
    string getIp(){ return m_ip; }


    void setInterface(string interface){ m_interface = interface; }
    string getInterface(){ return m_interface; }

    void addObserver(COAPObserver* observer);
private:

    map<string, COAPCallback> m_callbacks;
    vector<COAPObserver*> m_observers;

    string m_ip;
    string m_interface;
};

#endif // COAPPacket_H
