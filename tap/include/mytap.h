//tap设备封装的类

#if !defined(__H__MYTAP__H__)
#define __H__MYTAP__H__

#include <stddef.h>
#include <string>

class MyIP;

class MyMAC;

class MyMask;

class MyTap
{
private:
    /* data */
  MyMAC *mac_=nullptr;
  MyIP *ip_ = nullptr;
  MyMask *mask_ = nullptr;

  //适配器名字
  std::string name_;

  //设备句柄
  void *hd_=nullptr;

  //是否阻塞
  bool zs_ = true;

public:
  MyTap(/* args */);
  ~MyTap();

  //打开tap设备,返回0成功
  int Open(bool zs=true);

  //关闭tap设备,返回0成功
  int Close();

  //设置状态,返回0成功
  int SetStatus(bool st);

  //设置MAC
  int SetMac(const MyMAC *mac);

  //获取MAC
  const MyMAC *GetMAC() const;

  //设置IP
  int SetIP(MyIP *ip);

  //获取IP
  const MyIP *GetIP() const;

  //设置掩码
  int SetMask(const MyMask *mask);

  //获取掩码
  const MyMask *GetMask() const;

  //设置完mac,ip,掩码之后使用,返回0成功
  int SetTAP();

  //读取
  size_t Read(char *buf, size_t buflen);

  //写入
  size_t Write(const char *buf, size_t buflen);
};

#endif // __H__MYTAP__H__
