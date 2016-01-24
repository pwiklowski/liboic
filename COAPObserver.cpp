#include "COAPObserver.h"
#include "log.h"


COAPObserver::COAPObserver(string address, string href, vector<uint8_t> token)
{
    m_number = 5;
    log("New observer ");

    m_address = address;
    m_href = href;
    m_token = token;
}


void COAPObserver::notify(){

    log("notify");
}
