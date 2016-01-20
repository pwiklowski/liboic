#include "OICServer.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include "log.h"
#include <string>
#include "cbor.h"
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <net/if.h>

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
static bool lightCallback(COAPServer* server, COAPPacket* request, COAPPacket* response){
    static int temp =0;
    response->setType(COAP_TYPE_ACK);
    if (request->getHeader()->code == COAP_METHOD_GET){
        response->setResonseCode(COAP_RSPCODE_CONTENT);

        COAPOption* observeOption = request->getOption(COAP_OPTION_OBSERVE);
        if (observeOption)
        {
            server->addObserver(new COAPObserver(request->getUri(), request->getToken()));
        }


        cbor* value = cbor::map();
        value->append(new cbor("rt"), new cbor("oic.r.light.dimming"));
        value->append(new cbor("dimmingSetting"), new cbor(temp));
        value->append(new cbor("range"), new cbor("0,10000"));


        vector<uint8_t> data;
        data.push_back(((uint16_t)COAP_CONTENTTYPE_CBOR & 0xFF00) >> 8);
        data.push_back(((uint16_t)COAP_CONTENTTYPE_CBOR & 0xFF));

        response->addOption(new COAPOption(COAP_OPTION_CONTENT_FORMAT, data));

        value->dump(response->getPayload());
        delete value;

        return true;

    }else if(request->getHeader()->code == COAP_METHOD_POST){
        response->setResonseCode(COAP_RSPCODE_CHANGED);

        cbor* message = cbor::parse(request->getPayload());

        cbor* value = message->getMapValue("dimmingSetting");

        if (value != 0)
            temp = value->toInt();

        delete message;
        return true;

    }
    return false;

}

bool OICServer::onRequest(COAPServer* server, COAPPacket* request, COAPPacket* response){
    response->setType(COAP_TYPE_ACK);

    OICResource* resource = getResource(request->getUri());
    if (request->getHeader()->code == COAP_METHOD_GET){
        response->setResonseCode(COAP_RSPCODE_CONTENT);

        COAPOption* observeOption = request->getOption(COAP_OPTION_OBSERVE);
        if (observeOption)
        {
            server->addObserver(new COAPObserver(request->getUri(), request->getToken()));
        }

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
            resource->onUpdate(message);
        }

        delete message;
        return true;

    }
    return false;

}

OICServer::OICServer()
{

}
void OICServer::start(string ip, string interface){

    COAPServer coap_server;
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

    pthread_create(&m_thread, NULL, &OICServer::run, &coap_server);
    pthread_create(&m_thread, NULL, &OICServer::runDiscovery, &coap_server);
    pthread_join(m_thread, NULL);

}

void* OICServer::run(void* param){
    COAPServer* coap_server = (COAPServer*) param;

    int sockfd = socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in6 serv, client;
    struct ipv6_mreq mreq;
    const int on = 1;
    serv.sin6_family = AF_INET6;
    serv.sin6_port = htons(5683);
    serv.sin6_scope_id = if_nametoindex(coap_server->getInterface().c_str());


    if (inet_pton(AF_INET6, coap_server->getIp().c_str(), &(serv.sin6_addr)) < 0){
        log("Unable to bind");
        return 0;
    }
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
        }
        COAPPacket* p = new COAPPacket(buffer, rc);
        COAPPacket* response = coap_server->handleMessage(p);
        size_t response_len;
        response->build(buffer, &response_len);
        sendto(sockfd,buffer, response_len, 0, (struct sockaddr*)&client,l);
    }
}


void* OICServer::runDiscovery(void* param){
    COAPServer* coap_server = (COAPServer*) param;

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
        }
        COAPPacket* p = new COAPPacket(buffer, rc);
        COAPPacket* response = coap_server->handleMessage(p);
        size_t response_len;
        response->build(buffer, &response_len);
        sendto(sockfd,buffer, response_len, 0, (struct sockaddr*)&client,l);
    }
}

OICResource* OICServer::getResource(string href){
    for(uint16_t i=0; i<m_resources.size();i++){
        if (m_resources.at(i)->getHref() == href) return m_resources.at(i);
    }
    return 0;
}
