#ifndef COAPSERVER_H
#define COAPSERVER_H

#include <stdint.h>
#include <stddef.h>

typedef struct
{
    uint8_t ver;                /* CoAP version number */
    uint8_t t;                  /* CoAP Message Type */
    uint8_t tkl;                /* Token length: indicates length of the Token field */
    uint8_t code;               /* CoAP status code. Can be request (0.xx), success reponse (2.xx),
                                 * client error response (4.xx), or rever error response (5.xx)
                                 * For possible values, see http://tools.ietf.org/html/rfc7252#section-12.1 */
    uint8_t id[2];
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

typedef struct
{
    coap_header_t hdr;          /* Header of the packet */
    coap_buffer_t tok;          /* Token value, size as specified by hdr.tkl */
    uint8_t numopts;            /* Number of options */
    coap_option_t opts[16]; /* Options of the packet. For possible entries see
                                 * http://tools.ietf.org/html/rfc7252#section-5.10 */
    coap_buffer_t payload;      /* Payload carried by the packet */
} coap_packet_t;

class COAPServer
{
public:
    COAPServer();

    void handleMessage(uint8_t *data, size_t len);
    bool parseHeader(coap_header_t *hdr, const uint8_t *buf, size_t buflen);
    bool parseToken(coap_buffer_t *tokbuf, const coap_header_t *hdr, const uint8_t *buf, size_t buflen);
};

#endif // COAPSERVER_H
