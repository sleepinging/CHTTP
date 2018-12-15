//掩码类

#if !defined(__H__MYMASK__H__)
#define __H__MYMASK__H__

//#include <stdint.h>
#include <string>

class MyIP;

class MyMask
{
public:
    //最大32，所以只要一个字节
  unsigned char value;
  /* data */
public:
  MyMask(unsigned char v);
  MyMask();
  ~MyMask();

  //转为4字节格式的IPv4掩码,如255.255.248.0
  void ToIPv4Mask(unsigned char ipmask[4]);

  //获取网络地址,如192.168.9.138/21->192.168.8.0
  void ToNetIPv4(MyIP* ip,MyIP* net);

  //转字符串
  std::string ToString();
};

#endif // __H__MYMASK__H__
