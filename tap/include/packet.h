//解析数据包

#ifndef _PACKET_H_
#define _PACKET_H_ 1
#include <stdint.h>

#define NOT_IN_OUT 0
#define DIR_OUT 1
#define DIR_IN 2

class Packet
{
  public:

    unsigned char srcMAC[6];
    unsigned char dstMAC[6];
    unsigned char srcIP[4];
    unsigned char dstIP[4];
    uint16_t srcPort;
    uint16_t dstPort;
    unsigned char protocol;
    unsigned char payload[65535];
    uint32_t payloadLen;

    int Parse(const unsigned char *databuf, uint32_t len);

    void setSrcMAC(const unsigned char *mac);
    void setDstMAC(const unsigned char *mac);
    void setSrcIP(const unsigned char *ip);
    void setDstIP(const unsigned char *ip);
    void setPayload(const unsigned char *data, uint32_t datalen);

    void print();
};

#endif