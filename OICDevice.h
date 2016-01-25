#ifndef OICDEVICE_H
#define OICDEVICE_H

#include "pthread.h"
#include <stdint.h>
#include "OICResource.h"
#include "cbor.h"
#include <netinet/in.h>


#define IPV4



typedef void(*OICServerSendData)(uint8_t*, uint16_t);

class OICResource;


class OICDevice
{
public:
    OICDevice(string name);

    void start(string ip, string interface);
    void stop();

    void send(string destination, COAPPacket* packet, COAPResponseHandler func);
#ifdef IPV6
void send(sockaddr_in6 destination, COAPPacket* packet, COAPResponseHandler func);
#endif

#ifdef IPV4
void send(sockaddr_in destination, COAPPacket* packet, COAPResponseHandler func);
#endif

    COAPServer* getCoapServer() { return &coap_server; }

    int  getSocketFd() { return m_socketFd; }
    void setSocketFd(int socket) { m_socketFd = socket; }
protected:

#ifdef IPV6
    string convertAddress(sockaddr_in6 addr);
#endif

#ifdef IPV4
    string convertAddress(sockaddr_in addr);
#endif
    static void* run(void*param);

    OICResource* getResource(string href);

    pthread_t m_thread;

    uint8_t buffer[1024];
    COAPServer coap_server;
    int m_socketFd;
    string m_name;
};

#endif // OICSERVER_H
