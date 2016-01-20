#ifndef COAPOBSERVER_H
#define COAPOBSERVER_H

#include <string>
#include <vector>
#include <stdint.h>

using namespace std;

class COAPObserver
{
public:
    COAPObserver(string href, vector<uint8_t> token);

};

#endif // COAPOBSERVER_H
