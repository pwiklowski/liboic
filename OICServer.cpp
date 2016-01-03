#include "OICServer.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

OICServer::OICServer()
{


}

static void temperatureCallback(COAPPacket* request, COAPPacket* response){
    static int temp =0;
    response->setType(COAP_TYPE_ACK);
    if (request->getHeader()->code == COAP_METHOD_GET){
        response->setResonseCode(COAP_RSPCODE_CONTENT);

        string r = to_string(temp++);

        vector<uint8_t> data;
        data.push_back(((uint16_t)COAP_CONTENTTYPE_TEXT_PLAIN & 0xFF00) >> 8);
        data.push_back(((uint16_t)COAP_CONTENTTYPE_TEXT_PLAIN & 0xFF));

        response->addOption(new COAPOption(COAP_OPTION_CONTENT_FORMAT, data));
        response->addPayload(r);

    }else if(request->getHeader()->code == COAP_METHOD_POST){
        string value = request->getPayload();
        response->setResonseCode(COAP_RSPCODE_CHANGED);
        temp = atoi(value.c_str());

    }

}
void OICServer::start(){
    pthread_create(&m_thread, NULL, &OICServer::run, this);
    pthread_join(m_thread, NULL);
}
void* OICServer::run(void* param){
    OICServer* server = (OICServer*) param;

    COAPServer coap_server;
    coap_server.addEndpoint("/temp", "</temp>;rt=\"Temperature\"", temperatureCallback);
    coap_server.addEndpoint("/temp2", "</temp2>;rt=\"Temperature2\"", temperatureCallback);
    coap_server.addEndpoint("/temp3", "</temp3>;rt=\"Temperature3\"", temperatureCallback);
    coap_server.addEndpoint("/temp4", "</temp4>;rt=\"Temperature4\"", temperatureCallback);



    int sockfd;
    sockfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    struct sockaddr_in serv,client;

    serv.sin_family = AF_INET;
    serv.sin_port = htons(9999);
    serv.sin_addr.s_addr = htonl(INADDR_ANY);

    uint8_t buffer[1024];
    socklen_t l = sizeof(client);

    if( bind(sockfd , (struct sockaddr*)&serv, sizeof(serv) ) == -1)
    {
        std::cout << "Unable to bind";
        return 0;
    }

    while(1){
        size_t rc= recvfrom(sockfd,buffer,sizeof(buffer),0,(struct sockaddr *)&client,&l);
        if(rc<0)
        {
            std::cout<<"ERROR READING FROM SOCKET";
        }

        COAPPacket* p = new COAPPacket(buffer, rc);

        COAPPacket* response = coap_server.handleMessage(p);

        size_t response_len;

        response->build(buffer, &response_len);



        sendto(sockfd,buffer, response_len, 0, (struct sockaddr*)&client,l);
    }
}




