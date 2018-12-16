//一些网络数据类型定义

#if !defined(__H__NETDEF__H__)
#define __H__NETDEF__H__

#include <stdint.h>

//数据帧格式
//0x0000-0x05DC:IEEE 802.3
//0x0800:IPv4协议
//0x0806:ARP协议
//0x8100:VLAN
//0x86DD:IPv6
enum class PacketType
{
    IPv4 = 0x0800,
    ARP = 0x806,
    VLAN = 0x8100,
    IPv6 = 0x86DD
};

//传输层协议
enum class TransProtocol
{
    ICMP = 1,
    TCP = 6,
    UDP = 17
};

//MAC头，6+6+2=14字节
typedef struct eth_hdr
{
    unsigned char dst_mac[6]; //目的mac
    unsigned char src_mac[6]; //源mac

    //类型，一般在TCP/IP通信中只使用0800和0806这两种
    //0x0000-0x05DC:IEEE 802.3
    //0x0800:IPv4协议
    //0x0806:ARP协议
    //0x8100:VLAN
    //0x86DD:IPv6
    unsigned short eth_type;
} eth_hdr;

//IP头部，最少20字节，具体长度在IP头中
typedef struct ip_hdr
{
    int version : 4;            //版本
    int header_len : 4;         //IP头部长度，表示有多少个4字节，所以实际长度要*4
    unsigned char tos : 8;      //服务类型
    int total_len : 16;         //整个IP数据报的长度(含IP头部)
    int ident : 16;             //唯一的标识数据报。系统采用加1的式边发送边赋值
    int flags : 16;             //3位标识+13位分片偏移，为了方便对齐写一起了
    unsigned char ttl : 8;      //生存时间
    unsigned char protocol : 8; //原来区分上层协议，ICMP为1，TCP为6，UDP为17
    int checksum : 16;          //校验和
    unsigned char sourceIP[4];  //源IP
    unsigned char destIP[4];    //目的IP
} ip_hdr;

//TCP头，最少20字节，具体长度在TCP头中
typedef struct tcp_hdr
{
    unsigned short sport; //源端口
    unsigned short dport; //目的端口
    uint32_t seq;         //序号
    uint32_t ack;         //确认号

    //4位头部长度，表示有多少个4字节，所以实际长度要*4
    //4位保留，下面还有2位
    unsigned char head_len;

    //2位保留，接着上一个字段
    //6位标识
    //URG（紧急指针是否有效）
    //ACK（表示确认号是否有效）
    //PSH（提示接收端应用程序应该立即从TCP接收缓冲区读走数据）
    //RST（表示要求对方重新建立连接）
    //SYN（表示请求建立一个连接）
    //FIN（表示通知对方本端要关闭连接）
    unsigned char flags;

    unsigned short wind_size; //窗口大小
    unsigned short check_sum; //校验和
    unsigned short urg_ptr;   //紧急指针：它和序号段的值相加表示最后一个紧急数据的下一字节的序号
} tcp_hdr;

//UDP头，2+2+2+2=8字节
typedef struct udp_hdr
{
    unsigned short sport;     //源端口
    unsigned short dport;     //目的端口
    unsigned short tot_len;   //数据包长度
    unsigned short check_sum; //校验和
} udp_hdr;

#endif // __H__NETDEF__H__
