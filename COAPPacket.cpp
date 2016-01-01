#include "COAPPacket.h"
#include <cstdio>
#include  <cstring>

using namespace std;

COAPPacket::COAPPacket(uint8_t* data, size_t len)
{
    if (!parseHeader(&hdr, data, len))
        return;
    if (!parseToken(&tok, &hdr,data,len))
        return;
    if (!parseOptions(opts, &numopts, &hdr, data, len))
        return;


    printf("Header:\n");
    printf("  ver  0x%02X\n", hdr.ver);
    printf("  t    0x%02X\n", hdr.t);
    printf("  tkl  0x%02X\n", hdr.tkl);
    printf("  code 0x%02X\n", hdr.code);
    printf("  id   0x%02X%02X\n", hdr.id[0], hdr.id[1]);


    for(int i=0; i<numopts; i++)
    {
        printf("option %d\n", i);
        printf("    num %d\n", opts[i].num);
        printf("    len %d\n", opts[i].buf.len);
    }

}
bool COAPPacket::parseHeader(coap_header_t *hdr, const uint8_t *buf, size_t buflen)
{
    if (buflen < 4)
        return false;
    hdr->ver = (buf[0] & 0xC0) >> 6;
    if (hdr->ver != 1)
        return false;
    hdr->t = (buf[0] & 0x30) >> 4;
    hdr->tkl = buf[0] & 0x0F;
    hdr->code = buf[1];
    hdr->id[0] = buf[2];
    hdr->id[1] = buf[3];
    return true;
}
bool COAPPacket::parseToken(coap_buffer_t *tokbuf, const coap_header_t *hdr, const uint8_t *buf, size_t buflen)
{
    if (hdr->tkl == 0)
    {
        tokbuf->p = NULL;
        tokbuf->len = 0;
        return true;
    }
    else
    {
        if (hdr->tkl <= 8)
        {
            if (4U + hdr->tkl > buflen)
                return false;
            tokbuf->p = buf+4;
            tokbuf->len = hdr->tkl;
            return true;
        }
    }
    return false;
}
bool COAPPacket::parseOptions(coap_option_t *options, uint8_t *numOptions, const coap_header_t *hdr, const uint8_t *buf, size_t buflen)
{
    uint16_t delta = 0;
    const uint8_t *p = buf + 4 + hdr->tkl;
    const uint8_t *end = buf + buflen;
    uint8_t optionsNumber = 0;

    while((p < end) && (*p != 0xFF))
    {
        if (!parseOption(&options[optionsNumber], &delta, &p, end-p))
            return false;
        optionsNumber++;
    }
    *numOptions = optionsNumber;

    if (p+1 < end && *p == 0xFF)  // payload marker
    {
        //payload->p = p+1;
        //payload->len = end-(p+1);
    }

    return true;
}
bool COAPPacket::parseOption(coap_option_t *option, uint16_t *running_delta, const uint8_t **buf, size_t buflen)
{
    const uint8_t *p = *buf;
    uint8_t headlen = 1;
    uint16_t len, delta;

    if (buflen < headlen)
        return false;

    delta = (p[0] & 0xF0) >> 4;
    len = p[0] & 0x0F;

    if (len == 15){
        option->buf.p = p+2;
        option->buf.len = 15 + p[1];
        *buf = p + 2 + len;
    }else{
        option->buf.p = p+1;
        option->buf.len = len;
        *buf = p + 1 + len;
    }
    option->num = delta + *running_delta;

    *running_delta += delta;
    return true;
}


int COAPPacket::build(uint8_t *buf, size_t *buflen)
{
    size_t opts_len = 0;
    size_t i;
    uint8_t *p;
    uint16_t running_delta = 0;

    buf[0] = (hdr.ver & 0x03) << 6;
    buf[0] |= (hdr.t & 0x03) << 4;
    buf[0] |= (hdr.tkl & 0x0F);
    buf[1] = hdr.code;
    buf[2] = hdr.id[0];
    buf[3] = hdr.id[1];

    // inject token
    p = buf + 4;

    if (hdr.tkl > 0)
        std::memcpy(p, tok.p,hdr.tkl);

    p += hdr.tkl;



    for ( auto i = m_options.begin(); i != m_options.end(); i++ ) {

        COAPOption* option = &(*i);
        uint32_t optDelta;
        uint8_t len, delta = 0;

        optDelta = option->getNumber() - running_delta;

        if (option->getData().size()> 15){
            len = 15;
        }else{
            len = option->getData().size();
        }

        *p++ = (0xFF & (optDelta << 4 | len));
        if (delta == 14)
        {
            *p++ = ((optDelta-269) >> 8);
            *p++ = (0xFF & (optDelta-269));
        }
        if (option->getData().size() > 15)
        {
            *p++ = 0xFF & option->getData().size()-15;
        }

        for (int i=0; i<option->getData().size();i++){
            *(p++) = option->getData().at(i);
        }

        running_delta = option->getNumber();
    }

    opts_len = (p - buf) - 4;   // number of bytes used by options

    if (payload.length()> 0)
    {
        buf[4 + opts_len] = 0xFF;  // payload marker
        memcpy(buf+5 + opts_len, payload.c_str(), payload.length());
        *buflen = opts_len + 5 + payload.length();
    }
    else
        *buflen = opts_len + 4;
    return 0;
}


string COAPPacket::getUri(){
    string uri;
    for(int i=0; i<numopts; i++) {
        printf("option %d\n", i);
        if (opts[i].num == COAP_OPTION_URI_PATH){
            uri.append("/");
            string part((char*)opts[i].buf.p, opts[i].buf.len);

            uri.append(part);
        }
    }
    return uri;
}



COAPPacket::COAPPacket(COAPOption option, string content, uint8_t msgid_hi, uint8_t msgid_lo, const coap_buffer_t* token, coap_responsecode_t rspcode)
{
    hdr.ver = 0x01;
    hdr.t = COAP_TYPE_ACK;
    hdr.tkl = 0;
    hdr.code = rspcode;
    hdr.id[0] = msgid_hi;
    hdr.id[1] = msgid_lo;
    numopts = 1;

    // need token in response
    if (token) {
        hdr.tkl = token->len;
        tok = *token;
    }

    m_options.push_back(option);

    payload = content;
}
