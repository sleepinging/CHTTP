#include "mytap.h"

#include <Windows.h>

#include "tap-windows.h"

#include "mytool.h"
#include "myip.h"
#include "mymac.h"

using namespace std;

MyTap::MyTap(/* args */)
{
    mac_ = new MyMAC();
    ip_ = new MyIP();
    mask_ = new MyMASK();
}

MyTap::~MyTap()
{
    SafeDelete(mac_);
    SafeDelete(ip_);
    SafeDelete(mask_);
    CloseHandle(hd_);
}

//打开tap设备
int MyTap::Open(){
    auto id = GetTAPComponentId();
    id = "\\\\.\\Global\\" + id + ".tap";
    hd_ = CreateFileA(id.c_str(),
                                   GENERIC_READ | GENERIC_WRITE,
                                   FILE_SHARE_READ | FILE_SHARE_WRITE,
                                   nullptr,
                                   OPEN_EXISTING,
                                   FILE_ATTRIBUTE_NORMAL,
                                   nullptr);
    return (hd_ == INVALID_HANDLE_VALUE)?-1:0;
}

//设置状态
int MyTap::SetStatus(bool st){
    char code[4] = {0};
    code[0] = st ? 1 : 0;
    char buf[2048] = {0};
    DWORD n;
    return DeviceIoControl(hd_, TAP_WIN_IOCTL_SET_MEDIA_STATUS, code, 4, buf, 2048, &n, nullptr);
}

//设置MAC
int MyTap::SetMac(const MyMAC *mac){
    return 0;
}

//获取MAC
const MyMAC *MyTap::GetMAC() const{
    return 0;
}

//设置IP
int MyTap::SetIP(MyIP *ip){
    return 0;
}

//获取IP
MyIP *MyTap::GetIP() const{
    return 0;
}

//读取
size_t MyTap::Read(char *buf, size_t buflen){
    return 0;
}

//写入
size_t MyTap::Write(const char *buf, size_t buflen){
    return 0;
}