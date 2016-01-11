#include "CBORWriter.h"

CBORWriter::CBORWriter(vector<uint8_t>* data)
{
    m_data = data;
    m_pointer = 0;
}

CBORWriter::~CBORWriter()
{

}
void CBORWriter::writeType(int majorType, unsigned long long value) {
    majorType<<= 5;

    if(value < 24ULL) {
        m_data->push_back(majorType | value);
    } else if(value < 256ULL) {
       m_data->push_back(majorType | 24);
       m_data->push_back(value);
    } else if(value < 65536ULL) {
       m_data->push_back(majorType | 25);
       m_data->push_back(value >> 8);
    } else if(value < 4294967296ULL) {
       m_data->push_back(majorType | 26);
       m_data->push_back(value >> 24);
       m_data->push_back(value >> 16);
       m_data->push_back(value >> 8);
       m_data->push_back(value);
    } else {
       m_data->push_back(majorType | 27);
       m_data->push_back(value >> 56);
       m_data->push_back(value >> 48);
       m_data->push_back(value >> 40);
       m_data->push_back(value >> 32);
       m_data->push_back(value >> 24);
       m_data->push_back(value >> 16);
       m_data->push_back(value >> 8);
       m_data->push_back(value);
    }
}



void CBORWriter::writeString(const string str) {
    writeType(CBOR_STRING, str.size());

    for(uint16_t i=0; i<str.size();i++)
        m_data->push_back(str.at(i));
}

void CBORWriter::writeInt(long long value) {
    if(value < 0) {
        writeType(1, (unsigned long long) -(value+1));
    } else {
        writeType(0, (unsigned long long) value);
    }
}


void CBORWriter::writeArray(int size) {
    writeType(CBOR_ARRAY, size);
}

void CBORWriter::writeMap(int size) {
    writeType(CBOR_MAP, size);
}

void CBORWriter::writeTag(const unsigned int tag) {
    writeType(CBOR_TAG, tag);
}

void CBORWriter::writeSpecial(int special) {
    writeType(CBOR_SPECIAL, special);
}

