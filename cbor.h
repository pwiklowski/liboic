#ifndef CBOR_H
#define CBOR_H

#include <string>
#include <vector>
#include "map"

using namespace std;

enum CborType_t {
    CBOR_TYPE_UNSIGNED,
    CBOR_TYPE_NEGATIVE,
    CBOR_TYPE_BINARY,
    CBOR_TYPE_STRING,
    CBOR_TYPE_ARRAY,
    CBOR_TYPE_MAP,
    CBOR_TYPE_TAGGED,
    CBOR_TYPE_SIMPLE,
    CBOR_TYPE_FLOAT
};


class cbor{
public:

    static cbor* parse(vector<uint8_t>* data, uint16_t* p = 0){
        if(data->size() == 0)return 0;
        uint16_t pointer;
        if (p == 0)
            pointer = 0;
        else
            pointer = *p;

        uint8_t type = data->at(pointer++);
        CborType_t majorType = (CborType_t)(type >> 5);
        uint8_t minorType = (unsigned char) (type & 31);

        uint64_t value =0;
        if(minorType < 24) {
            value = minorType;
        } else if(minorType == 24) { // 1 byte
            value = data->at(pointer++);
        } else if(minorType == 25) { // 2 byte
            value = ((unsigned short) data->at(pointer) << 8) | ((unsigned short) data->at(pointer+ 1));
            pointer += 2;
        } else if(minorType == 26) { // 4 byte
            value = ((unsigned int) data->at(pointer) << 24) |
                  ((unsigned int) data->at(pointer+ 1) << 16) |
                  ((unsigned int) data->at(pointer+ 2) << 8 ) |
                  ((unsigned int) data->at(pointer+ 3));
            pointer +=4;
        } else {

        }

        cbor* aa = new cbor(majorType);

        if (majorType == CBOR_TYPE_ARRAY){
            for (uint16_t i=0; i<value; i++)
                aa->append(cbor::parse(data, &pointer));
        }else if (majorType == CBOR_TYPE_STRING){
            for (uint16_t i=0; i<value; i++)
                aa->data()->push_back(data->at(pointer++));
        }else if (majorType == CBOR_TYPE_MAP){
            for (uint16_t i=0; i<value; i++){
                cbor* key = cbor::parse(data, &pointer);
                cbor* value = cbor::parse(data, &pointer);
                aa->append(key, value);
            }
        }else if (majorType == CBOR_TYPE_NEGATIVE || majorType == CBOR_TYPE_UNSIGNED){
            if(value < 256ULL) {
                aa->data()->push_back(value);
            } else if(value < 65536ULL) {
                aa->data()->push_back(value >> 8);
                aa->data()->push_back(value);
            } else if(value < 4294967296ULL) {
                aa->data()->push_back(value >> 24);
                aa->data()->push_back(value >> 16);
                aa->data()->push_back(value >> 8);
                aa->data()->push_back(value);
            } else {
                aa->data()->push_back(value >> 56);
                aa->data()->push_back(value >> 48);
                aa->data()->push_back(value >> 40);
                aa->data()->push_back(value >> 32);
                aa->data()->push_back(value >> 24);
                aa->data()->push_back(value >> 16);
                aa->data()->push_back(value >> 8);
                aa->data()->push_back(value);
            }

        }


        if (p!=0)
            *p = pointer;


        return aa;
    }
    ~cbor(){
        m_data.clear();

        for(uint16_t i=0;i<m_array.size();i++){
            delete m_array.at(i);
        }
        for(auto key = m_map.begin(); key!=m_map.end(); key++){
            pair<cbor*, cbor*> p = *key;

            delete p.first;
            delete p.second;
        }

    }

    cbor(string str) {
        m_type = CBOR_TYPE_STRING;
        for(uint16_t i=0; i<str.size();i++)
            m_data.push_back(str.at(i));
    }

    cbor(long long v) {
        unsigned long long value;
        if (value < 0){
            m_type = CBOR_TYPE_NEGATIVE;
            value = -(value+1);
        } else{
            value = v;
            m_type = CBOR_TYPE_UNSIGNED;
        }

        if(value < 256ULL) {
            m_data.push_back(value);
        } else if(value < 65536ULL) {
            m_data.push_back(value >> 8);
            m_data.push_back(value);
        } else if(value < 4294967296ULL) {
            m_data.push_back(value >> 24);
            m_data.push_back(value >> 16);
            m_data.push_back(value >> 8);
            m_data.push_back(value);
        } else {
            m_data.push_back(value >> 56);
            m_data.push_back(value >> 48);
            m_data.push_back(value >> 40);
            m_data.push_back(value >> 32);
            m_data.push_back(value >> 24);
            m_data.push_back(value >> 16);
            m_data.push_back(value >> 8);
            m_data.push_back(value);
        }



    }

    cbor(vector<cbor*> array) {
        m_type = CBOR_TYPE_ARRAY;
        m_array = array;
    }

    static cbor* array(){
        return new cbor(CBOR_TYPE_ARRAY);
    }

    static cbor* map(){
        return new cbor(CBOR_TYPE_MAP);
    }

    cbor(CborType_t type){
        m_type = type;
    }

    CborType_t getType(){
        return m_type;
    }

    vector< pair<cbor*, cbor*> >* toMap() {
        if (m_type == CBOR_TYPE_MAP)
            return &m_map;
        else
            return 0;
    }


   cbor* getMapValue(string key) {
       if (m_type != CBOR_TYPE_MAP)
           return 0;

       for(auto item = m_map.begin();item !=m_map.end(); item++){
           pair<cbor*, cbor*> p = *item ;

           if (p.first->compare(key)){
                return p.second;
           }
       }
       return 0;

    }

   //workd only for strings
    bool compare(string str){
        if (str.size() != m_data.size())
            return false;

        for (uint16_t i=0; i<str.size(); i++){
            if (str[i] != m_data[i])
                return false;
        }
        return true;
    }

    void append(cbor* key, cbor* value){
        m_map.push_back(make_pair(key, value));
    }
    void append(cbor* value){
        m_array.push_back(value);
    }

    vector<cbor*>* toArray() {
        if (m_type == CBOR_TYPE_ARRAY)
            return &m_array;
        else
            return 0;
    }

    string toString(){
        string out;
        for(uint16_t i=0; i<m_data.size();i++)
            out += m_data.at(i);
        return out;
    }

    int toInt(){
        int value;

        if(m_data.size() == 1) {
            value = m_data.at(0);
        } else if(m_data.size()== 2) { // 2 byte
            value = (m_data.at(0) << 8) | m_data.at(1);
        } else if(m_data.size() == 4) { // 4 byte
            value = (m_data.at(0) << 24) |
                    (m_data.at(1) << 16) |
                    (m_data.at(2) << 8 ) |
                    (m_data.at(3));
        }
        if (m_type ==CBOR_TYPE_NEGATIVE)
            value = -(value+1);
        return value;
    }

    vector<uint8_t>* data(){
        return &m_data;
    }


    void dump(vector<uint8_t>* data){
       uint8_t majorType = m_type << 5;

       unsigned long long value = 0;
       if (m_type == CBOR_TYPE_ARRAY){
           value = m_array.size();
       }else if (m_type == CBOR_TYPE_MAP){
           value = m_map.size();
       }else if (m_type == CBOR_TYPE_STRING){
           value = m_data.size();
       }else if (m_type == CBOR_TYPE_UNSIGNED || m_type == CBOR_TYPE_NEGATIVE){
            if (m_data.size() == 1){
                if (m_data.at(0) < 24){
                    data->push_back(majorType | m_data.at(0));
                }else{
                    data->push_back(majorType | 24);
                    data->push_back(m_data.at(0));
                }
            }else if (m_data.size() > 1){

                if (m_data.size() == 2) data->push_back(majorType | 25);
                if (m_data.size() == 4) data->push_back(majorType | 26);
                if (m_data.size() == 8) data->push_back(majorType | 27);

                for (uint16_t i=0; i<m_data.size(); i++) data->push_back(m_data.at(i));
            }
       }

       if (m_type != CBOR_TYPE_UNSIGNED && m_type != CBOR_TYPE_NEGATIVE){
           if(value < 24ULL) {
               data->push_back(majorType | value);
           } else if(value < 256ULL) {
               data->push_back(majorType | 24);
               data->push_back(value);
           } else if(value < 65536ULL) {
               data->push_back(majorType | 25);
               data->push_back(value >> 8);
               data->push_back(value);
           } else if(value < 4294967296ULL) {
               data->push_back(majorType | 26);
               data->push_back(value >> 24);
               data->push_back(value >> 16);
               data->push_back(value >> 8);
               data->push_back(value);
           } else {
               data->push_back(majorType | 27);
               data->push_back(value >> 56);
               data->push_back(value >> 48);
               data->push_back(value >> 40);
               data->push_back(value >> 32);
               data->push_back(value >> 24);
               data->push_back(value >> 16);
               data->push_back(value >> 8);
               data->push_back(value);
           }
       }


       if (m_type == CBOR_TYPE_ARRAY){
           for(uint16_t i=0;i<m_array.size();i++){
               m_array.at(i)->dump(data);
           }
       } else if (m_type == CBOR_TYPE_MAP){
           for(auto key = m_map.begin(); key!=m_map.end(); key++){
               pair<cbor*, cbor*> p = *key;

               p.first->dump(data);
               p.second->dump(data);
           }
       } else if (m_type == CBOR_TYPE_STRING){
            for(uint16_t i=0; i<m_data.size();i++)
                data->push_back(m_data.at(i));

       }



    }

    bool operator <(const cbor& rhs) const
    {
        return true;
    }
    bool is_string(){
        if (m_type == CBOR_TYPE_STRING)
            return true;
        else
            return false;

    }

    bool is_int(){
        if (m_type == CBOR_TYPE_NEGATIVE || m_type == CBOR_TYPE_UNSIGNED)
            return true;
        else
            return false;
    }

private:
    CborType_t m_type;
    vector<uint8_t> m_data;
    vector<cbor*> m_array;
    vector< pair<cbor*, cbor*> > m_map;
};


#endif // cbor_H
