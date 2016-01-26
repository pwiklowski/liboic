#ifndef COAPPACKET_H
#define COAPPACKET_H

#include <stdint.h>
#include <stddef.h>
#include <string>
#include "COAPOption.h"


typedef struct
{
    uint8_t ver;                /* CoAP version number */
    uint8_t t;                  /* CoAP Message Type */
    uint8_t tkl;                /* Token length: indicates length of the Token field */
    uint8_t code;               /* CoAP status code. Can be request (0.xx), success reponse (2.xx),
                                 * client error response (4.xx), or rever error response (5.xx)
                                 * For possible values, see http://tools.ietf.org/html/rfc7252#section-12.1 */
    uint16_t mid;
} coap_header_t;

typedef struct
{
    const uint8_t *p;
    size_t len;
} coap_buffer_t;

typedef struct
{
    uint8_t *p;
    size_t len;
} coap_rw_buffer_t;

typedef struct
{
    uint8_t num;                /* Option number. See http://tools.ietf.org/html/rfc7252#section-5.10 */
    coap_buffer_t buf;          /* Option value */
} coap_option_t;


typedef enum
{
    COAP_METHOD_GET = 1,
    COAP_METHOD_POST = 2,
    COAP_METHOD_PUT = 3,
    COAP_METHOD_DELETE = 4
} coap_method_t;

typedef enum
{
    COAP_TYPE_CON = 0,
    COAP_TYPE_NONCON = 1,
    COAP_TYPE_ACK = 2,
    COAP_TYPE_RESET = 3
} coap_msgtype_t;


typedef enum
{
    COAP_OPTION_IF_MATCH = 1,
    COAP_OPTION_URI_HOST = 3,
    COAP_OPTION_ETAG = 4,
    COAP_OPTION_IF_NONE_MATCH = 5,
    COAP_OPTION_OBSERVE = 6,
    COAP_OPTION_URI_PORT = 7,
    COAP_OPTION_LOCATION_PATH = 8,
    COAP_OPTION_URI_PATH = 11,
    COAP_OPTION_CONTENT_FORMAT = 12,
    COAP_OPTION_MAX_AGE = 14,
    COAP_OPTION_URI_QUERY = 15,
    COAP_OPTION_ACCEPT = 17,
    COAP_OPTION_LOCATION_QUERY = 20,
    COAP_OPTION_PROXY_URI = 35,
    COAP_OPTION_PROXY_SCHEME = 39
} coap_option_num_t;

#define MAKE_RSPCODE(clas, det) ((clas << 5) | (det))
typedef enum
{
    COAP_RSPCODE_CONTENT = MAKE_RSPCODE(2, 5),
    COAP_RSPCODE_NOT_FOUND = MAKE_RSPCODE(4, 4),
    COAP_RSPCODE_FORBIDDEN = MAKE_RSPCODE(4, 3),
    COAP_RSPCODE_BAD_REQUEST = MAKE_RSPCODE(4, 0),
    COAP_RSPCODE_CHANGED = MAKE_RSPCODE(2, 4)
} coap_responsecode_t;

typedef enum
{
    COAP_CONTENTTYPE_NONE = -1, // bodge to allow us not to send option block
    COAP_CONTENTTYPE_TEXT_PLAIN = 0,
    COAP_CONTENTTYPE_APPLICATION_LINKFORMAT = 40,
    COAP_CONTENTTYPE_CBOR = 60,
} coap_content_type_t;

class COAPPacket
{
public:
    COAPPacket(uint8_t *data, size_t len, string address);
    COAPPacket();

    std::string getUri();

    int build(uint8_t *buf, size_t *buflen);

    coap_header_t* getHeader(){ return &hdr;}
    vector<uint8_t> getToken(){ return m_token;}

    vector<uint8_t>* getPayload(){ return &m_payload; }


    void addOption(COAPOption* option){ m_options.push_back(option);}
    void addPayload(vector<uint8_t> payload){ m_payload = payload; }
    void addPayload(string payload);
    void addPayload(uint8_t* payload, uint16_t size);

    void setType(uint16_t type){ hdr.t = type;}
    void setResonseCode(uint8_t responseCode){ hdr.code = responseCode; }

    string getAddress() { return m_address;}
    void setAddress(string address) { m_address = address;}

    void setToken(uint16_t token){ hdr.tkl = 2; m_token.push_back(token); m_token.push_back(token >> 8); }
    void setToken(vector<uint8_t> token){ hdr.tkl = token.size(); m_token = token; }
    void setMessageId(uint16_t id){hdr.mid = id;}


    COAPOption* getOption(coap_option_num_t option);

    static void parseUri(COAPPacket* p, string uri);
private:
    coap_header_t hdr;          /* Header of the packet */
    vector<uint8_t> m_token;          /* Token value, size as specified by hdr.tkl */
    vector<uint8_t> m_payload;
    vector<COAPOption*> m_options;
    string m_address;

    bool parseHeader(coap_header_t *hdr, const uint8_t *buf, size_t buflen);
    bool parseToken(const coap_header_t *hdr, const uint8_t *buf, size_t buflen);
    bool parseOptions(const coap_header_t *hdr, const uint8_t *buf, size_t buflen);
    bool parseOption(uint16_t *running_delta, const uint8_t **buf, size_t buflen);


};


#endif // COAPPACKET_H
