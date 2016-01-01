#ifndef OICSERVER_H
#define OICSERVER_H

#include "pthread.h"
#include <stdint.h>

typedef void(*OICServerSendData)(uint8_t*, uint16_t);

class OICServer
{
public:
    OICServer();

    void start();

private:
    static void* run(void*param);

    pthread_t m_thread;
};

#endif // OICSERVER_H
