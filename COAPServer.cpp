#include "COAPServer.h"
#include <cstdio>
#include <string.h>

using namespace std;

COAPServer::COAPServer()
{
    printf("%s \n", "af");
}



COAPPacket* COAPServer::handleMessage(COAPPacket* p){


    string uri = p->getUri();
    printf("handleRequest %s\n", uri.c_str());

    if (p->getHeader()->code == COAP_METHOD_GET){

        if (uri.compare("/.well-known/core") == 0){

        }
    }

     string r = "</temp>;rt=\"Temperature\";ct=0";


    vector<uint8_t> data;
    data.push_back(((uint16_t)COAP_CONTENTTYPE_APPLICATION_LINKFORMAT & 0xFF00) >> 8);
    data.push_back(((uint16_t)COAP_CONTENTTYPE_APPLICATION_LINKFORMAT & 0xFF));

    COAPOption o(COAP_OPTION_CONTENT_FORMAT, data);



    return new COAPPacket(o, r, p->getHeader()->id[0], p->getHeader()->id[1], p->getToken(), COAP_RSPCODE_CONTENT);
}





