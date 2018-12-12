//tap设备封装的类

#if !defined(__H__MYTAP__H__)
#define __H__MYTAP__H__

#include <stddef.h>

class MyIP;

class MyMAC;

class MyTap
{
private:
    /* data */
  MyMAC *mac_;
  MyIP *ip_;

public:
  MyTap(/* args */);
  ~MyTap();

  //打开tap设备
  int Open();

  //设置状态
  int SetStatus(bool st);

  //设置MAC
  int SetMac(const MyMAC *mac);

  //获取MAC
  const MyMAC *GetMAC() const;

  //设置IP
  int SetIP(MyIP *ip);

  //获取IP
  MyIP *GetIP() const;

  //读取
  size_t Read(char *buf, size_t buflen);

  //写入
  size_t Write(const char *buf, size_t buflen);
};

#endif // __H__MYTAP__H__
