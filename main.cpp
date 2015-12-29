#include "OICServer.h"

int main(){

    OICServer* server = new OICServer();
    server->start();

    return 0;
}
