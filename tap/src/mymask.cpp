#include "mymask.h"

#include <stdint.h>

#include "myip.h"

using namespace std;

MyMask::MyMask(unsigned char v)
    :value(v)
{
}

MyMask::MyMask()
{
}

MyMask::~MyMask()
{
}

//转为4字节格式的IPv4掩码
void MyMask::ToIPv4Mask(unsigned char ipmask[4]) const
{
    uint32_t b=0xffffffff;
    b <<= (32 - value);
    ipmask[3] = b;
    ipmask[2] = b >> 8;
    ipmask[1] = b >> 16;
    ipmask[0] = b >> 24;
}

//获取网络地址,如192.168.9.138/21->192.168.8.0
void MyMask::ToNetIPv4(const MyIP *ip, MyIP *net) const
{
    uint32_t b = 0xffffffff;
    b <<= (32 - value);
    uint32_t ipb = 0;
    ipb |= ip->data[0];
    ipb <<= 8;
    ipb |= ip->data[1];
    ipb <<= 8;
    ipb |= ip->data[2];
    ipb <<= 8;
    ipb |= ip->data[3];

    uint32_t rb = ipb & b;
    net->data[3] = rb;
    net->data[2] = rb >> 8;
    net->data[1] = rb >> 16;
    net->data[0] = rb >> 24;
}

//转字符串
std::string MyMask::ToString() const
{
    MyIP ip;
    this->ToIPv4Mask(ip.data);
    return ip.ToString();
}