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

  unsigned short mtu_ = 0;

  //id
  std::string id_;

  //完整设备名
  std::string dname_;

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

  //设置状态(连接|断开),返回0成功
  int SetStatus(bool st);

  //设置MAC
  int SetMac(const MyMAC *mac);

  //设置MTU
  int SetMTU(unsigned short mtu);

  //获取MAC
  const MyMAC *GetMAC() const;

  //设置IP和掩码,返回0成功
  int SetIPMask(MyIP *ip,MyMask* mask);

  //获取IP
  const MyIP *GetIP() const;

  //获取掩码
  const MyMask *GetMask() const;

  //启用tap设备
  int SetEnable();

  //禁用tap设备
  int SetDisable();

  //设置完mac,ip,掩码之后使用,返回0成功
  int SetTAP();

  //读取
  size_t Read(char *buf, size_t buflen);

  //写入
  size_t Write(const char *buf, size_t buflen);
};

#endif // __H__MYTAP__H__
