#include "OICServer.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <iostream>
#include "log.h"
#include <string>
#include "cbor.h"
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <net/if.h>
#include "COAPObserver.h"


bool OICServer::discoveryRequest(COAPServer* server, COAPPacket* request, COAPPacket* response){
    response->setType(COAP_TYPE_ACK);
    if (request->getHeader()->code == COAP_METHOD_GET){
        response->setResonseCode(COAP_RSPCODE_CONTENT);

        vector<uint8_t> p;

        cbor* root = new cbor(CBOR_TYPE_ARRAY);
        cbor* device = new cbor(CBOR_TYPE_MAP);

        device->append(new cbor("di"), new cbor("0685B960-736F-46F7-BEC0-9E6CBD61ADC1"));
        device->append(new cbor("n"), new cbor("OIC Sample Device"));

        cbor* links = cbor::array();

        for(uint16_t i=0; i<m_resources.size();i++){
            cbor* val = cbor::map();

            val->append(new cbor("href"), new cbor(m_resources.at(i)->getHref()));
            val->append(new cbor("rt"), new cbor(m_resources.at(i)->getResourceType()));
            val->append(new cbor("if"), new cbor(m_resources.at(i)->getInterface()));
            val->append(new cbor("type"), new cbor("application/cbor"));

            links->append(val);
        }

        device->append(new cbor("links"), links);

        root->append(device);

        root->dump(&p);

        delete root;

        vector<uint8_t> data;
        data.push_back(((uint16_t)COAP_CONTENTTYPE_CBOR & 0xFF00) >> 8);
        data.push_back(((uint16_t)COAP_CONTENTTYPE_CBOR & 0xFF));

        response->addOption(new COAPOption(COAP_OPTION_CONTENT_FORMAT, data));
        response->addPayload(p);

        return true;
    }
    return false;

}
bool OICServer::onRequest(COAPServer* server, COAPPacket* request, COAPPacket* response){
    response->setType(COAP_TYPE_ACK);

    OICResource* resource = getResource(request->getUri());
    if (request->getHeader()->code == COAP_METHOD_GET){
        response->setResonseCode(COAP_RSPCODE_CONTENT);


        if (resource != 0){
            resource->value()->dump(response->getPayload());
        }

        vector<uint8_t> data;
        data.push_back(((uint16_t)COAP_CONTENTTYPE_CBOR & 0xFF00) >> 8);
        data.push_back(((uint16_t)COAP_CONTENTTYPE_CBOR & 0xFF));

        response->addOption(new COAPOption(COAP_OPTION_CONTENT_FORMAT, data));

        return true;

    }else if(request->getHeader()->code == COAP_METHOD_POST){
        response->setResonseCode(COAP_RSPCODE_CHANGED);

        cbor* message = cbor::parse(request->getPayload());

        if (resource != 0){
            resource->update(message, true);
        }

        delete message;
        return true;

    }
    return false;

}

OICServer::OICServer():
    coap_server([&](string dest, COAPPacket* packet, COAPResponseHandler handler){
        this->send(dest, packet, handler);
    })
{

}
void OICServer::start(string ip, string interface){

    coap_server.setInterface(interface);
    coap_server.setIp(ip);

    COAPCallback discoveryCallback = [=](COAPServer* server, COAPPacket* request, COAPPacket* response) {
        return this->discoveryRequest(server, request, response);
    };

    coap_server.addResource("/oic/res", discoveryCallback);

    for(uint16_t i=0; i<m_resources.size();i++){
        coap_server.addResource(m_resources.at(i)->getHref(), [=](COAPServer* server, COAPPacket* request, COAPPacket* response) {
            return this->onRequest(server, request, response);
        });
    }

    pthread_create(&m_thread, NULL, &OICServer::run, this);

#ifdef IPV6
    pthread_create(&m_discoveryThread, NULL, &OICServer::runDiscovery, this);
#endif
}

void OICServer::stop(){
    pthread_join(m_thread, NULL);
}

void* OICServer::run(void* param){
    OICServer* oic_server = (OICServer*) param;
    COAPServer* coap_server = oic_server->getCoapServer();


    const int on = 1;
#ifdef IPV6
    int fd = socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in6 serv, client;
    serv.sin6_family = AF_INET6;
    serv.sin6_port = htons(5683);
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
    serv.sin_port = htons(5683);
    serv.sin_addr.s_addr = htonl(INADDR_ANY);


    mreq.imr_multiaddr.s_addr=inet_addr("224.0.1.187");
    mreq.imr_interface.s_addr=htonl(INADDR_ANY);
    if (setsockopt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0) {
        return 0;
    }
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
        COAPPacket* p = new COAPPacket(buffer, rc, oic_server->convertAddress(client));
        coap_server->handleMessage(p);
    }
}

#ifdef IPV6
void* OICServer::runDiscovery(void* param){
    OICServer* oic_server = (OICServer*) param;
    COAPServer* coap_server = oic_server->getCoapServer();

    int sockfd = socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in6 serv, client;
    struct ipv6_mreq mreq;
    const int on = 1;
    serv.sin6_family = AF_INET6;
    serv.sin6_port = htons(5683);
    serv.sin6_scope_id = if_nametoindex(coap_server->getInterface().c_str());


    serv.sin6_addr = in6addr_any;
    uint8_t buffer[1024];
    socklen_t l = sizeof(client);
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
    {
        log("Unable to set reuse");
        return 0;
    }
    if( bind(sockfd , (struct sockaddr*)&serv, sizeof(serv) ) == -1)
    {
        log("Unable to bind");
        return 0;
    }

    if (inet_pton(AF_INET6, "ff02::fd", &(mreq.ipv6mr_multiaddr)) < 0){
        log("Unable to bind");
        return 0;
    }
    mreq.ipv6mr_interface = if_nametoindex(coap_server->getInterface().c_str());

    if (setsockopt(sockfd,IPPROTO_IPV6, IPV6_JOIN_GROUP, &mreq, sizeof(mreq)) < 0) {
        log("unable to make it listen for multicast");
       return 0;
    }

    while(1){
        size_t rc= recvfrom(sockfd,buffer,sizeof(buffer),0,(struct sockaddr *)&client,&l);
        if(rc<0)
        {
            std::cout<<"ERROR READING FROM SOCKET";
            continue;
        }
        COAPPacket* p = new COAPPacket(buffer, rc, oic_server->convertAddress(client));

        coap_server->handleMessage(p);
    }
}
#endif


#ifdef IPV6
string OICServer::convertAddress(sockaddr_in6 addr){
    string port = to_string(htons(addr.sin6_port));
    char a[30];

    if (inet_ntop(AF_INET6, (sockaddr_in6*)&addr.sin6_addr, a, 30) == 0){
        log("Unable to bind");
        return 0;
    }

    string address(a);
    address += ' ';
    address += port;
    return address;
}
#endif


#ifdef IPV4
string OICServer::convertAddress(sockaddr_in addr){
    string port = to_string(htons(addr.sin_port));
    char a[30];

    if (inet_ntop(AF_INET, (sockaddr_in*)&addr.sin_addr, a, 30) == 0){
        log("Unable to bind");
        return 0;
    }

    string address(a);
    address += ' ';
    address += port;
    return address;
}
#endif

OICResource* OICServer::getResource(string href){
    for(uint16_t i=0; i<m_resources.size();i++){
        if (m_resources.at(i)->getHref() == href) return m_resources.at(i);
    }
    return 0;
}



void OICServer::send(string destination, COAPPacket* packet, COAPResponseHandler func){
    std::size_t pos = destination.find(" ");
    string ip = destination.substr(0, pos);
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


    send(client, packet, func);
}
#ifdef IPV6
void OICServer::send(sockaddr_in6 destination, COAPPacket* packet, COAPResponseHandler func){
#endif

#ifdef IPV4
void OICServer::send(sockaddr_in destination, COAPPacket* packet, COAPResponseHandler func){
#endif

    if (func !=0)
        coap_server.addResponseHandler(packet->getHeader()->mid, func);

    uint8_t buffer[1024];
    size_t response_len;
    socklen_t l = sizeof(destination);
    packet->build(buffer, &response_len);
    sendto(m_socketFd, buffer, response_len, 0, (struct sockaddr*)&destination, l);
}
