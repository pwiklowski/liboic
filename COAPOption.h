#ifndef COAPOPTION_H
#define COAPOPTION_H

#include <vector>
#include <stdint.h>

using namespace std;


class COAPOption
{
public:
    COAPOption(uint8_t num, vector<uint8_t> data);
    COAPOption(uint8_t num, char *data);
    uint8_t getNumber() { return m_num;}
    vector<uint8_t>* getData() { return &m_data;}

private:
    uint8_t m_num;
    vector<uint8_t> m_data;
};

#endif // COAPOPTION_H
