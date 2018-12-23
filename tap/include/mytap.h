//tap设备封装的类

#if !defined(__H__MYTAP__H__)
#define __H__MYTAP__H__

#include <stddef.h>
#include <string>

#ifdef _WIN32
#define nullhandle nullptr
using MYHANDLE = void *;
#else
using MYHANDLE = int;
#define nullhandle -1
#endif

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
  MYHANDLE hd_ = nullhandle;

  //是否阻塞
  bool zs_ = true;

public:
  MyTap(/* args */);
  ~MyTap();
  MyTap(const MyTap &tap);
  MyTap(MyTap &&tap);

public:
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

  //使用TUN,设置完mac,ip,掩码之后使用,返回0成功
  int SetTUN();

  //读取,TAP从第二层(MAC层)开始,TUN从第三层(IP层)开始
  size_t Read(char *buf, size_t buflen);

  //写入,TAP从第二层(MAC层)开始,TUN从第三层(IP层)开始
  size_t Write(const char *buf, size_t buflen);
private:
  //打开tun设备,返回0成功,unix使用
  int opentun(bool zs = true);

public:
  static MyTap NewTAP(MyMAC *mac, MyIP* ip,MyMask* mask);
  static MyTap NewTUN(MyIP *ip, MyMask *mask);
};

#endif // __H__MYTAP__H__
