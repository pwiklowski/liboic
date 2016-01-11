#include "CBORReader.h"



CBORReader::CBORReader(vector<uint8_t>* data)
{
    m_pointer = 0;
    m_data = data;
}
int CBORReader::readArray(){
    pair<uint8_t, uint16_t> type = readType();

    if (type.first == CBOR_ARRAY){
        return type.second;
    }
    return -1;
}

pair<uint8_t,uint16_t> CBORReader::readType(){
    uint8_t type = m_data->at(m_pointer++);
    uint8_t majorType = type >> 5;
    uint8_t minorType = (unsigned char) (type & 31);

    uint16_t len=0;
    if(minorType < 24) {
        len = minorType;
    } else if(minorType == 24) { // 1 byte
        len = getByte();
    } else if(minorType == 25) { // 2 byte
        len = getShort();
    } else if(minorType == 26) { // 4 byte
        len = getInt();
    } else {
        len =-1;
    }

    return make_pair<uint8_t, uint16_t>((uint8_t)majorType,(uint16_t) len);
}

int CBORReader::readMap(){
    pair<uint8_t, uint16_t> type = readType();

    if (type.first == CBOR_MAP){
        return type.second;
    }
    return -1;
}

string CBORReader::readString(){
    pair<uint8_t, uint16_t> type = readType();
    string s;

    if (type.first == CBOR_STRING){
        int len = type.second;

        for(int i=0; i<len; i++){
            s += getByte();
        }

    }
    return s;
}


CBORReader::~CBORReader()
{

}

uint8_t CBORReader::getByte() {
    return m_data->at(m_pointer++);
}



uint8_t CBORReader::getTypeMajor(uint8_t type) {
    return type >>5;
}

uint8_t CBORReader::getTypeMinor(uint8_t type) {

    return type & 31;
}

uint16_t CBORReader::getShort() {
    uint16_t value = ((unsigned short) m_data->at(m_pointer) << 8) | ((unsigned short) m_data->at(m_pointer+ 1));
    m_pointer += 2;
    return value;
}

uint32_t CBORReader::getInt() {
    uint32_t value = \
            ((unsigned int) m_data->at(m_pointer) << 24) |
            ((unsigned int) m_data->at(m_pointer+ 1) << 16) |
            ((unsigned int) m_data->at(m_pointer+ 2) << 8 ) |
            ((unsigned int) m_data->at(m_pointer+ 3));
    m_pointer += 4;
    return value;
}

//unsigned long long input::get_long() {
//    unsigned long long value = ((unsigned long long) _data[_offset] << 56) |
//            ((unsigned long long) _data[_offset +1] << 48) | ((unsigned long long) _data[_offset +2] << 40) |
//            ((unsigned long long) _data[_offset +3] << 32) | ((unsigned long long) _data[_offset +4] << 24) |
//            ((unsigned long long) _data[_offset +5] << 16) | ((unsigned long long) _data[_offset +6] << 8 ) |
//            ((unsigned long long) _data[_offset +7]);
//    _offset += 8;
//    return value;
//}
