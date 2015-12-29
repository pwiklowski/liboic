#include "COAPServer.h"
#include <cstdio>

COAPServer::COAPServer()
{
}



void COAPServer::handleMessage(uint8_t* data, size_t len){

    printf("handleMessage\n");
    coap_packet_t packet;

    if (!parseHeader(&packet.hdr, data, len))
        return;
    if (!parseToken(&packet.tok, &packet.hdr,data,len))
        return;

}


bool COAPServer::parseHeader(coap_header_t *hdr, const uint8_t *buf, size_t buflen)
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

    printf("Header:\n");
    printf("  ver  0x%02X\n", hdr->ver);
    printf("  t    0x%02X\n", hdr->t);
    printf("  tkl  0x%02X\n", hdr->tkl);
    printf("  code 0x%02X\n", hdr->code);
    printf("  id   0x%02X%02X\n", hdr->id[0], hdr->id[1]);

    return true;
}
bool COAPServer::parseToken(coap_buffer_t *tokbuf, const coap_header_t *hdr, const uint8_t *buf, size_t buflen)
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
            tokbuf->p = buf+4;  // past header
            tokbuf->len = hdr->tkl;
            return true;
        }
    }
    return false;
}
