#include "mytap.h"

#include <Windows.h>

#include "tap-windows.h"

#include "mytool.h"
#include "myip.h"
#include "mymac.h"
#include "mymask.h"

using namespace std;

MyTap::MyTap(/* args */)
{
    mac_ = new MyMAC();
    ip_ = new MyIP();
    mask_ = new MyMask();
}

MyTap::~MyTap()
{
    SafeDelete(mac_);
    SafeDelete(ip_);
    SafeDelete(mask_);
    CloseHandle(hd_);
}

//打开tap设备
int MyTap::Open(bool zs)
{
    auto id = GetTAPComponentId();
    id = "\\\\.\\Global\\" + id + ".tap";
    hd_ = CreateFileA(
        id.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | zs ? 0 : FILE_FLAG_OVERLAPPED,
        nullptr);
    zs_ = zs;
    return (hd_ == INVALID_HANDLE_VALUE) ? -1 : 0;
}

//关闭tap设备
int MyTap::Close()
{
    return CloseHandle(hd_);
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

    *mac_ = *mac;
    return 0;
}

//获取MAC
const MyMAC *MyTap::GetMAC() const{
    return mac_;
}

//设置IP
int MyTap::SetIP(MyIP *ip){

    *ip_ = *ip;
    return 0;
}

//获取IP
const MyIP *MyTap::GetIP() const
{
    return ip_;
}

//设置掩码
int MyTap::SetMask(const MyMask *mask)
{

    *mask_ = *mask;
    return 0;
}

//获取掩码
const MyMask *MyTap::GetMask() const
{
    return mask_;
}

//设置完mac,ip,掩码之后使用
int MyTap::SetTAP()
{
    DWORD rt = 0;
    unsigned char code[12] = {0};
    
    return 0;
}

//读取
size_t MyTap::Read(char *buf, size_t buflen){
    DWORD rd = 0;
    if (zs_)
    {
        ReadFile(hd_, buf, buflen, &rd, nullptr);
    }
    return rd;
}

//写入
size_t MyTap::Write(const char *buf, size_t buflen){
    DWORD wd = 0;
    if (zs_)
    {
        WriteFile(hd_, buf, buflen, &wd, nullptr);
    }
    return wd;
}