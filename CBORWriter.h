#ifndef CBORWRITER_H
#define CBORWRITER_H

#include "vector"
#include "stdint.h"
#include "string"

#define CBOR_STRING 3
#define CBOR_ARRAY 4
#define CBOR_MAP 5
#define CBOR_TAG 6
#define CBOR_SPECIAL 7

using namespace std;


class CBORWriter
{
public:
    CBORWriter(vector<uint8_t>* data);
    ~CBORWriter();

    void writeType(int majorType, unsigned long long value);
    void writeArray(int size);
    void writeMap(int size);
    void writeTag(const unsigned int tag);
    void writeSpecial(int special);
    void writeInt(long long value);
    void writeString(const string str);

private:
    vector<uint8_t>* m_data;
    uint16_t m_pointer;

};

#endif // CBORWRITER_H
