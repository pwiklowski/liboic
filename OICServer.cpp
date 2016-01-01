#include "OICServer.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include "COAPServer.h"

OICServer::OICServer()
{


}
void OICServer::start(){
    pthread_create(&m_thread, NULL, &OICServer::run, this);
    pthread_join(m_thread, NULL);
}
void* OICServer::run(void* param){
    COAPServer coap_server;
    OICServer* server = (OICServer*) param;
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





