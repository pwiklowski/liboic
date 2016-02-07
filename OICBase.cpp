#include "OICBase.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <iostream>
#include "log.h"
#include "cbor.h"
#include <netdb.h>
#include <stdio.h>
#include "String.h"
#include <net/if.h>
#include "COAPObserver.h"


OICBase::OICBase(String name):
    coap_server([&](COAPPacket* packet, COAPResponseHandler handler){
        this->send(packet, handler);
    })
{
    m_name = name;
    m_is_client = false;
}
void OICBase::start(String ip, String interface){

    coap_server.setInterface(interface);
    coap_server.setIp(ip);

    pthread_create(&m_thread, NULL, &OICBase::run, this);

#ifdef IPV6
    pthread_create(&m_discoveryThread, NULL, &OICBase::runDiscovery, this);
#endif
}

void OICBase::stop(){
    pthread_join(m_thread, NULL);
}

void* OICBase::run(void* param){
    OICBase* oic_server = (OICBase*) param;
    COAPServer* coap_server = oic_server->getCoapServer();


    const int on = 1;
#ifdef IPV6
    int fd = socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in6 serv, client;
    serv.sin6_family = AF_INET6;
    serv.sin6_port = 0;
    serv.sin6_scope_id = if_nametoindex(coap_server->getInterface().c_str());

    if (inet_pton(AF_INET6, coap_server->getIp().c_str(), &(serv.sin6_addr)) < 0){
        log("Unable to bind");
        return 0;
    }
#endif


#ifdef IPV4
    int fd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    struct sockaddr_in serv,client;
    struct ip_mreq mreq;

    serv.sin_family = AF_INET;
    serv.sin_port = 0;
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
#endif

    oic_server->setSocketFd(fd);

    uint8_t buffer[1024];
    socklen_t l = sizeof(client);
    if(setsockopt(oic_server->getSocketFd(), SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
    {
        log("Unable to set reuse");
        return 0;
    }
    if( bind(oic_server->getSocketFd(), (struct sockaddr*)&serv, sizeof(serv) ) == -1)
    {
        log("Unable to bind");
        return 0;
    }

    while(1){
        size_t rc= recvfrom(oic_server->getSocketFd(),buffer,sizeof(buffer),0,(struct sockaddr *)&client,&l);
        if(rc<0)
        {
            std::cout<<"ERROR READING FROM SOCKET";
        }
        COAPPacket* p = new COAPPacket(buffer, rc, oic_server->convertAddress(client).c_str());
        coap_server->handleMessage(p);
    }
}

#ifdef IPV6
String OICBase::convertAddress(sockaddr_in6 addr){
    String port = to_String(htons(addr.sin6_port));
    char a[30];

    if (inet_ntop(AF_INET6, (sockaddr_in6*)&addr.sin6_addr, a, 30) == 0){
        log("Unable to bind");
        return 0;
    }

    String address(a);
    address += ' ';
    address += port;
    return address;
}
#endif


#ifdef IPV4
String OICBase::convertAddress(sockaddr_in addr){
    String port = to_string(htons(addr.sin_port)).c_str();
    char a[30];

    if (inet_ntop(AF_INET, (sockaddr_in*)&addr.sin_addr, a, 30) == 0){
        log("Unable to bind");
        return 0;
    }

    String address(a);
    address.append(' ');
    address.append(port);
    return address;
}
#endif

void OICBase::send(COAPPacket* packet, COAPResponseHandler func){
    String destination = packet->getAddress();
    size_t pos = destination.find(" ");
    String ip = destination.substr(0, pos);
    uint16_t port = atoi(destination.substr(pos).c_str());

#ifdef IPV6

    struct sockaddr_in6 client;
    client.sin6_family = AF_INET6;
    client.sin6_port = htons(port);
    client.sin6_scope_id = if_nametoindex(coap_server.getInterface().c_str());

    if (inet_pton(AF_INET6, ip.c_str(), &(client.sin6_addr)) < 0){
        log("err");
    }
#endif

#ifdef IPV4

    struct sockaddr_in client;
    client.sin_family = AF_INET;
    client.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &(client.sin_addr)) < 0){
        log("err");
    }
#endif
    if (packet->getHeader()->mid !=0)
        packet->getHeader()->mid = m_id++;

    send(client, packet, func);
}
#ifdef IPV6
void OICBase::send(sockaddr_in6 destination, COAPPacket* packet, COAPResponseHandler func){
#endif

#ifdef IPV4
void OICBase::send(sockaddr_in destination, COAPPacket* packet, COAPResponseHandler func){
#endif

    if (func !=0)
        coap_server.addResponseHandler(packet->getHeader()->mid, func);

    uint8_t buffer[1024];
    size_t response_len;
    socklen_t l = sizeof(destination);
    packet->build(buffer, &response_len);
    sendto(m_socketFd, buffer, response_len, 0, (struct sockaddr*)&destination, l);
}
