#ifndef COAPServer_H
#define COAPServer_H

#include <string>
#include "COAPPacket.h"
#include <vector>
#include <map>
#include <functional>
#include <iostream>

class COAPServer;
class COAPObserver;

typedef std::function<void(COAPPacket*)> COAPResponseHandler;
typedef std::function<bool(COAPServer*, COAPPacket*, COAPPacket*)> COAPCallback;
typedef std::function<void(COAPPacket* packet, COAPResponseHandler func)> COAPSend;


class COAPServer
{
public:
    COAPServer(COAPSend sender);

    void handleMessage(COAPPacket* p);
    void addResource(string url, COAPCallback callback);
    void addResponseHandler(uint16_t messageId, COAPResponseHandler handler);

    void setIp(string ip){ m_ip = ip; }
    string getIp(){ return m_ip; }


    void setInterface(string interface){ m_interface = interface; }
    string getInterface(){ return m_interface; }

    void notify(string href, vector<uint8_t> data);
private:
    COAPSend m_sender;
    map<string, COAPCallback> m_callbacks;
    map<uint16_t, COAPResponseHandler> m_responseHandlers;

    string m_ip;
    string m_interface;
    uint16_t m_port;


    vector<COAPObserver*> m_observers;
};

#endif // COAPPacket_H
