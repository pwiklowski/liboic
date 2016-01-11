#ifndef CBORREADER_H
#define CBORREADER_H

#include <vector>
#include <stdint.h>
#include <string>

using namespace std;

#define CBOR_STRING 3
#define CBOR_ARRAY 4
#define CBOR_MAP 5
#define CBOR_TAG 6

class CBORReader
{
public:
    CBORReader(vector<uint8_t>* data);
    ~CBORReader();

    int readArray();
    int readMap();
    string readString();


    uint8_t getByte();
    uint16_t getShort();
    uint32_t getInt();

    pair<uint8_t,uint16_t> readType();

    static uint8_t getTypeMajor(uint8_t type);
    static uint8_t getTypeMinor(uint8_t type);
private:
    vector<uint8_t>* m_data;
    uint16_t m_pointer;
};

#endif // CBORREADER_H
