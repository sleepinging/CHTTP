#include "mytap.h"

MyTap::MyTap(/* args */)
{
}

MyTap::~MyTap()
{
}

//打开tap设备
int MyTap::Open(){

}

//设置状态
int MyTap::SetStatus(bool st){}

//设置MAC
int MyTap::SetMac(const MyMAC *mac){}

//获取MAC
const MyMAC *MyTap::GetMAC() const{}

//设置IP
int MyTap::SetIP(MyIP *ip){}

//获取IP
MyIP *MyTap::GetIP() const{}

//读取
size_t MyTap::Read(char *buf, size_t buflen){}

//写入
size_t MyTap::Write(const char *buf, size_t buflen){}