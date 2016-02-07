#include "OICServer.h"
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


bool OICServer::discoveryRequest(COAPServer* server, COAPPacket* request, COAPPacket* response){
    response->setType(COAP_TYPE_ACK);
    if (request->getCode() == COAP_METHOD_GET){
        response->setResonseCode(COAP_RSPCODE_CONTENT);

        List<uint8_t> p;

        cbor* root = new cbor(CBOR_TYPE_ARRAY);
        cbor* device = new cbor(CBOR_TYPE_MAP);

        device->append(new cbor("di"), new cbor("0685B960-736F-46F7-BEC0-9E6CBD61ADC1"));
        device->append(new cbor("n"), new cbor(m_name));

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

        List<uint8_t> data;
        data.append(((uint16_t)COAP_CONTENTTYPE_CBOR & 0xFF00) >> 8);
        data.append(((uint16_t)COAP_CONTENTTYPE_CBOR & 0xFF));

        response->addOption(new COAPOption(COAP_OPTION_CONTENT_FORMAT, data));
        response->addPayload(p);
        response->setMessageId(request->getMessageId());

        return true;
    }
    return false;

}

bool OICServer::onRequest(COAPServer* server, COAPPacket* request, COAPPacket* response){
    response->setType(COAP_TYPE_ACK);

    OICResource* resource = getResource(request->getUri());
    if (request->getCode() == COAP_METHOD_GET){
        response->setResonseCode(COAP_RSPCODE_CONTENT);


        if (resource != 0){
            resource->value()->dump(response->getPayload());
        }

        List<uint8_t> data;
        data.append(((uint16_t)COAP_CONTENTTYPE_CBOR & 0xFF00) >> 8);
        data.append(((uint16_t)COAP_CONTENTTYPE_CBOR & 0xFF));

        response->addOption(new COAPOption(COAP_OPTION_CONTENT_FORMAT, data));

        return true;

    }else if(request->getCode() == COAP_METHOD_POST){
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
void OICServer::start(String ip, String interface){

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
    pthread_create(&m_discoveryThread, NULL, &OICServer::runDiscovery, this);
}

void* OICServer::runDiscovery(void* param){
    OICServer* oic_server = (OICServer*) param;
    COAPServer* coap_server = oic_server->getCoapServer();

    const int on = 1;
#ifdef IPV6
    int fd = socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in6 serv, client;
    serv.sin6_family = AF_INET6;
    serv.sin6_port = htons(5683);
    serv.sin6_scope_id = if_nametoindex(coap_server->getInterface().c_str());

    serv.sin6_addr = in6addr_any;
    if (inet_pton(AF_INET6, coap_server->getIp().c_str(), &(serv.sin6_addr)) < 0){
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
#endif


#ifdef IPV4
    int fd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    struct sockaddr_in serv,client;
    struct ip_mreq mreq;

    serv.sin_family = AF_INET;
    if (oic_server->isClient())
        serv.sin_port = 0;
    else
        serv.sin_port = htons(5683);
    serv.sin_addr.s_addr = htonl(INADDR_ANY);

    if (!oic_server->isClient()){
        mreq.imr_multiaddr.s_addr=inet_addr("224.0.1.187");
        mreq.imr_interface.s_addr=htonl(INADDR_ANY);
        if (setsockopt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0) {
            return 0;
        }
    }
#endif

    uint8_t buffer[1024];
    socklen_t l = sizeof(client);
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
    {
        log("Unable to set reuse");
        return 0;
    }
    if( bind(fd , (struct sockaddr*)&serv, sizeof(serv) ) == -1)
    {
        log("Unable to bind");
        return 0;
    }


    while(1){
        size_t rc= recvfrom(fd,buffer,sizeof(buffer),0,(struct sockaddr *)&client,&l);
        log("discovery thread received packet");
        if(rc<0)
        {
            std::cout<<"ERROR READING FROM SOCKET";
            continue;
        }
        COAPPacket* p = new COAPPacket(buffer, rc, oic_server->convertAddress(client));

        coap_server->handleMessage(p);
    }
}


OICResource* OICServer::getResource(String href){
    for(uint16_t i=0; i<m_resources.size();i++){
        if (m_resources.at(i)->getHref() == href) return m_resources.at(i);
    }
    return 0;
}

