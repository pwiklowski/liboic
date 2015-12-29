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

    char buffer[256];
    socklen_t l = sizeof(client);

    if( bind(sockfd , (struct sockaddr*)&serv, sizeof(serv) ) == -1)
    {
    }

    while(1){
        int rc= recvfrom(sockfd,buffer,sizeof(buffer),0,(struct sockaddr *)&client,&l);
        if(rc<0)
        {
            std::cout<<"ERROR READING FROM SOCKET";
        }
        std::cout<<"\n the message received is : "<<buffer<<std::endl;
        coap_server.handleMessage((uint8_t*)buffer, rc);
    }
}
