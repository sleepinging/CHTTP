#include "packet.h"

#include <stdio.h>
#include <string.h>

#include "netdef.hpp"

void Packet::setSrcMAC(const unsigned char *mac)
{
    memcpy(srcMAC, mac, 6);
}
void Packet::setDstMAC(const unsigned char *mac)
{
    memcpy(dstMAC, mac, 6);
}
void Packet::setSrcIP(const unsigned char *ip)
{
    memcpy(srcIP, ip, 4);
}
void Packet::setDstIP(const unsigned char *ip)
{
    memcpy(dstIP, ip, 4);
}
void Packet::setPayload(const unsigned char *data, uint32_t datalen)
{
    memcpy(payload, data, datalen);
    payloadLen = datalen;
}

unsigned char length_check(uint32_t totallen, uint32_t index, uint32_t leastlen)
{
    if (index + leastlen > totallen)
        return 0;
    else
        return 1;
}

int Packet::Parse(const unsigned char *databuf, uint32_t len)
{
    // 进行简单的包分析
    uint32_t index = 0;

    //检查index后面的包长度是否至少有14字节（检查至二层TYPE）
    if (!length_check(len, index, 14))
        return -1;

    // 取源MAC地址，目的MAC地址
    const unsigned char *dstMAC = databuf;
    index += 6;
    const unsigned char *srcMAC = databuf + index;
    index += 6;

    if (databuf[index + 1] != 0x00)
        return -1;
    if (databuf[index] == 0x08) //IPV4包
        index += 2;
    if (databuf[index] == 0x81) //802.1Q VLAN包
        index += 6;

    //检查index后面的包长度是否至少有24字节（检查至四层目的端口）
    if (!length_check(len, index, 24))
        return -1;

    // 检查是否IPv4
    if ((databuf[index] >> 4) != 4)
        return -1;
    index++;

    // 跳过 dsf(1), totallen(2), id(2), flags(1), offset(1), ttl(1) = 8
    index += 1 + 2 + 2 + 1 + 1 + 1;

    //获取协议类型（TCP/UDP）
    unsigned char protocol = databuf[index];
    index++;
    // 跳过 checksum(2)
    index += 2;

    //获取源IP和目的IP，源端口和目的端口
    const unsigned char *srcIP = databuf + index;
    index += 4;
    const unsigned char *dstIP = databuf + index;
    index += 4;
    uint16_t srcPort = (databuf[index] << 8) + databuf[index + 1];
    index += 2;
    uint16_t dstPort = (databuf[index] << 8) + databuf[index + 1];
    index += 2;

    const unsigned char *payload = NULL;
    uint32_t payloadLen = 0;

    if (protocol == static_cast<unsigned short>(TransProtocol::TCP))
    {
        // 跳过seq number(4), acknumber(4)
        index += 4 + 4;

        //检查index后面的包长度是否至少有2字节（检查至TCP 标志位）
        if (!length_check(len, index, 2))
            return -1;

        uint32_t tcpheaderlen = (databuf[index] & 0xf0) >> 2; // (databuf[index] >> 4) * 4
        index++;

        // 如果是TCP包，判断标志是否是SYN,RST,FIN
        if (
            databuf[index] &
            (static_cast<int>(TCPFlag::SYN) |
             static_cast<int>(TCPFlag::RST) |
             static_cast<int>(TCPFlag::FIN)))
        {
            return -1;
        }
        index++;

        // 跳过 window size(2), check sum(2),urgent pointer(2)
        index += 2 + 2 + 2;

        // 跳过 option(tcpheaderlen - 20)
        index += tcpheaderlen - 20;

        //检查index后面的包长度是否至少有1字节
        if (!length_check(len, index, 1))
            return -1;
        payload = databuf + index;
        payloadLen = len - index;
    }
    else
    {
        // 跳过 udplen(2), checksum(2)
        index += 2 + 2;

        //检查index后面的包长度是否至少有1字节
        if (!length_check(len, index, 1))
            return -1;

        payload = databuf + index;
        payloadLen = len - index;
    }

    Packet *p = this;
    p->setSrcMAC(srcMAC);
    p->setDstMAC(dstMAC);
    p->setSrcIP(srcIP);
    p->setDstIP(dstIP);
    p->srcPort = srcPort;
    p->dstPort = dstPort;
    p->protocol = protocol == static_cast<unsigned short>(TransProtocol::TCP) ? 0 : 1; // 0 tcp, 1 udp 与golang版本一致
    p->setPayload(payload, payloadLen);

    return 0;
}

void Packet::print()
{
    printf("srcmac: %02x:%02x:%02x:%02x:%02x:%02x\n", srcMAC[0], srcMAC[1], srcMAC[2], srcMAC[3], srcMAC[4], srcMAC[5]);
    printf("dstmac: %02x:%02x:%02x:%02x:%02x:%02x\n", dstMAC[0], dstMAC[1], dstMAC[2], dstMAC[3], dstMAC[4], dstMAC[5]);
    printf("srcip: %d.%d.%d.%d\n", srcIP[0], srcIP[1], srcIP[2], srcIP[3]);
    printf("dstip: %d.%d.%d.%d\n", dstIP[0], dstIP[1], dstIP[2], dstIP[3]);
}