#ifndef COAPOBSERVER_H
#define COAPOBSERVER_H

#include <string>
#include <vector>
#include <stdint.h>
#include <functional>
#include "COAPServer.h"


using namespace std;

class COAPObserver
{
public:
    COAPObserver(string address, string href, vector<uint8_t> token);
    COAPObserver(string address, string href, vector<uint8_t> token, COAPResponseHandler handler);
    void notify();

    string getHref() { return m_href; }
    string getAddress() { return m_address; }
    vector<uint8_t> getToken() { return m_token; }

    uint8_t getNumber() { return m_number++;}

    void handle(COAPPacket* p){m_handler(p);}

private:
    string m_href;
    string m_address;
    vector<uint8_t> m_token;
    uint8_t m_number;
    COAPResponseHandler m_handler;
};

#endif // COAPOBSERVER_H
