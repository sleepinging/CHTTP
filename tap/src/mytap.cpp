#include "mytap.h"

#include <string.h>

#include <iostream>

#ifdef _WIN32
#include <WinSock2.h>
#include "tap-windows.h"
#else
using DWORD = unsigned long;
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <linux/if_tun.h>
#include <unistd.h>
#endif

#include "mytool.h"
#include "mystring.h"
#include "myip.h"
#include "mymac.h"
#include "mymask.h"
#include "myipnet.h"
#include "config.h"

using namespace std;

MyTap::MyTap(/* args */)
{
    mac_ = new MyMAC();
    ip_ = new MyIP();
    mask_ = new MyMask();

#ifdef _WIN32
    auto cfg = Config::GetInstance();
    id_ = GetTAPComponentId(cfg->Adapter_Key);
    if(id_==""){
        cout << "get adapter id failed!" << endl;
        throw "get adapter id failed!";
    }
    dname_ = USERMODEDEVICEDIR + id_ + TAP_WIN_SUFFIX;
    name_ = GetRegValue(cfg->NetWork_Connections_Key + ("\\"+id_) + "\\Connection", "Name");
    if (name_ == "")
    {
        cout << "get adapter name failed!" << endl;
        throw "get adapter name failed!";
    }
    // //最好先使用netsh interface show interface [名字]
    // //判断状态，直接设置启用也可以
    // this->SetEnable();
#else

#endif
}

MyTap::~MyTap()
{
    SafeDelete(mac_);
    SafeDelete(ip_);
    SafeDelete(mask_);
#ifdef _WIN32
    CloseHandle(hd_);
#else

#endif
}

// MyTap::MyTap(const MyTap &tap)
// {
//     *mac_ = *tap.mac_;
//     *ip_ = *tap.ip_;
//     *mask_ = *tap.mask_;
//     mtu_ = tap.mtu_;
//     id_ = tap.id_;
//     name_ = tap.name_;
//     dname_ = tap.dname_;
//     hd_ = tap.hd_;
//     zs_ = tap.zs_;
// }

MyTap::MyTap(MyTap &&tap)
{
    mac_ = tap.mac_;
    tap.mac_ = nullptr;
    ip_ = tap.ip_;
    tap.ip_ = nullptr;
    mask_ = tap.mask_;
    tap.mask_ = nullptr;

    id_ = std::move(tap.id_);
    name_ = std::move(tap.name_);
    dname_ = std::move(tap.dname_);

    mtu_ = tap.mtu_;
    hd_ = tap.hd_;
    zs_ = tap.zs_;
}

MyTap &MyTap::operator=(MyTap &&tap)
{
    mac_ = tap.mac_;
    tap.mac_ = nullptr;
    ip_ = tap.ip_;
    tap.ip_ = nullptr;
    mask_ = tap.mask_;
    tap.mask_ = nullptr;

    id_ = std::move(tap.id_);
    name_ = std::move(tap.name_);
    dname_ = std::move(tap.dname_);

    mtu_ = tap.mtu_;
    hd_ = tap.hd_;
    zs_ = tap.zs_;

    return *this;
}

//打开tap设备
int MyTap::Open(bool zs)
{
    zs_ = zs;
#ifdef _WIN32
    hd_ = CreateFileA(
        dname_.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | (zs ? 0 : FILE_FLAG_OVERLAPPED),
        nullptr);
    
    if (hd_ == INVALID_HANDLE_VALUE){
        return -1;
    }
#else
    struct ifreq ifr;
    int fd, err;

    auto cfg = Config::GetInstance();
    const char *clonedev = cfg->LinuxTUNPath.c_str();

    // IFF_TUN:
    //     创建一个点对点设备
    // IFF_TAP:
    //     创建一个以太网设备
    // IFF_NO_PI:
    //     不包含包信息，默认的每个数据包当传到用户空间时，都将包含一个附加的包头来保存包信息
    // IFF_ONE_QUEUE:
    //     采用单一队列模式，即当数据包队列满的时候，由虚拟网络设备自已丢弃以后的数据包直到数据包队列再有空闲。
    int flags = IFF_TAP | IFF_NO_PI | (zs ? 0 : IFF_ONE_QUEUE);

    if ((fd = open(clonedev, O_RDWR)) < 0)
    {
        perror("open clonedev failed");
        return fd;
    }

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = flags;

    if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0)
    {
        perror("ioctl failed");
        close(fd);
        return err;
    }

    // printf("Open tun/tap device: %s for reading...\n", ifr.ifr_name);

    hd_ = fd;
    name_ = ifr.ifr_name;

#endif
    //cout << name_ << endl;
    return 0;
}

int MyTap::opentun(bool zs)
{
    int r = 0;
    zs_ = zs;
#ifdef _WIN32
#else
    struct ifreq ifr;
    int fd, err;
    const char *clonedev = "/dev/net/tun";

    int flags = IFF_TUN | IFF_NO_PI | (zs ? 0 : IFF_ONE_QUEUE);

    if ((fd = open(clonedev, O_RDWR)) < 0)
    {
        perror("open failed");
        return fd;
    }

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = flags;

    if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0)
    {
        perror("ioctl failed");
        close(fd);
        return err;
    }

    // printf("Open tun/tap device: %s for reading...\n", ifr.ifr_name);

    hd_ = fd;
    name_ = ifr.ifr_name;
#endif
    return r;
}

//关闭tap设备
int MyTap::Close()
{
#ifdef _WIN32
    int r = CloseHandle(hd_);
    if(r==0){
        return -1;
    }
#else
#endif
    return 0;
}

//设置状态
int MyTap::SetStatus(bool st){
#ifdef _WIN32
    char code[4] = {0};
    code[0] = st ? 1 : 0;
    char buf[2048] = {0};
    DWORD n;
    int r = DeviceIoControl(hd_, TAP_WIN_IOCTL_SET_MEDIA_STATUS, code, 4, buf, 2048, &n, nullptr);
    if(r==0){
        return -1;
    }
#else
    st = st;
#endif
    return 0;
}

//设置MAC
int MyTap::SetMac(const MyMAC *mac){
    *mac_ = *mac;
    int r = 0;
#ifdef _WIN32
    r = SetRegValueString(ADAPTER_KEY, "MAC", mac_->ToUp('\0'));
#else
    //sudo ifconfig eth0 hw ether 00:15:58:c2:85:c8
    string cmd = "ifconfig " + name_ + " hw ether "+mac_->ToUp(':');
    r = ExecCmd({cmd});
#endif
    return r;
}

//设置MTU
int MyTap::SetMTU(unsigned short mtu)
{
    mtu_ = mtu;
    int r = 0;
#ifdef _WIN32
    r = SetRegValueString(ADAPTER_KEY, "MTU", mytrans<unsigned short, string>(mtu_));
#else
    //sudo ifconfig tap0 mtu 1400
    string cmd = "ifconfig " + name_ + " mtu " + mytrans<unsigned short, string>(mtu);
    r = ExecCmd({cmd});
#endif
    return r;
}

//获取MAC
const MyMAC *MyTap::GetMAC() const{
    return mac_;
}

//设置IP和掩码
int MyTap::SetIPMask(const MyIP *ip,const MyMask *mask)
{
    *ip_ = *ip;
    *mask_ = *mask;
    int r = 0;
#ifdef _WIN32
    string namecmd = "\"" + name_ + "\"";
    r=ExecCmd({"netsh", "interface", "ip", "set", "address", namecmd, "static", ip_->ToString(), mask_->ToString()});
#else
    //ip addr add 192.168.209.138/24 dev tun0
    string namecmd = "ip addr add " + ip_->ToString() + "/" + mytrans<int, string>(mask_->value)+" dev "+name_;
    r = ExecCmd({namecmd});
#endif
    return r;
}

//设置IP和掩码,返回0成功
int MyTap::SetIPMask(const MyIPNet *ipnet){
    int r = 0;
    *ip_ = ipnet->GetIP();
    *mask_ = ipnet->GetMask();
    r = this->SetIPMask(ip_, mask_);
    return r;
}

//设置IP和掩码,返回0成功
int MyTap::SetIPMask(const std::string &ipnetstr)
{
    int r = 0;
    try
    {
        MyIPNet ipnet(ipnetstr);
        r = this->SetIPMask(&ipnet);
    }catch(...){

    }
    return r;
}

//获取IP
const MyIP *MyTap::GetIP() const
{
    return ip_;
}

//获取掩码
const MyMask *MyTap::GetMask() const
{
    return mask_;
}

//使用TUN,设置完ip,掩码之后使用
int MyTap::SetTUN()
{
#ifdef _WIN32
    DWORD rt = 0;
    unsigned char code[12] = {0};
    memcpy(code, ip_->data, 4);
    MyIP net;
    mask_->ToNetIPv4(ip_, &net);
    memcpy(code + 4, net.data, 4);
    mask_->ToIPv4Mask(code + 8);
    char buf[2048] = {0};
    int r = DeviceIoControl(hd_, TAP_WIN_IOCTL_CONFIG_TUN, code, 12, buf, 2048, &rt, nullptr);
    if (r == 0)
    {
        return -1;
    }
#else
    // struct ifreq ifr;
    // int err;
    // int flags = IFF_TUN | IFF_NO_PI | (zs_ ? 0 : IFF_ONE_QUEUE);
    // memset(&ifr, 0, sizeof(ifr));
    // ifr.ifr_flags = flags;

    // if ((err = ioctl(hd_, TUNSETIFF, (void *)&ifr)) < 0)
    // {
    //     perror("ioctl failed");
    //     close(hd_);
    //     return err;
    // }
    // name_ = ifr.ifr_name;
#endif
    return 0;
}

//启用tap设备
int MyTap::SetEnable()
{
    int r = 0;
#ifdef _WIN32
    string namecmd = "\"" + name_ + "\"";
    r = ExecCmd({"netsh", "interface", "set", "interface", namecmd, "ENABLED"});
#else
    //ip link set dev tun0 up
    string namecmd = "ip link set dev " + name_ + " up";
    r = ExecCmd({namecmd});
#endif
    return r;
}

//禁用tap设备
int MyTap::SetDisable()
{
    int r = 0;
#ifdef _WIN32
    string namecmd = "\"" + name_ + "\"";
    r = ExecCmd({"netsh", "interface", "set", "interface", namecmd, "DISABLED"});
#else
    //ip link set dev tun0 down
    string namecmd = "ip link set dev " + name_ + " down";
    r = ExecCmd({namecmd});
#endif
    return r;
}

#ifdef _WIN32
int getOverlappedResult(HANDLE hd, LPOVERLAPPED poa)
{
    DWORD n;
    if (GetOverlappedResult(hd, poa, &n, 1) == FALSE)
    {
        return -1;
    }
    return n;
}

int newOverlapped(LPOVERLAPPED poa)
{
    HANDLE h= CreateEventA(0, 1, 0, 0);
    if(h==nullptr){
        return -1;
    }
    poa->hEvent = h;
    return 1;
}

#endif

//读取
size_t MyTap::Read(char *buf, size_t buflen){
    size_t r = 0;
#ifdef _WIN32
    DWORD rd = 0;
    if (zs_)
    {
        ReadFile(hd_, buf, buflen, &rd,nullptr);
        r = rd;
    }else{
        OVERLAPPED poa;
        newOverlapped(&poa);
        if (ResetEvent(poa.hEvent) == FALSE)
        {
            cout << "ResetEvent failed" << endl;
            return r;
        }
        ReadFile(hd_, buf, buflen, &rd, &poa);
        r = getOverlappedResult(hd_, &poa);
    }
#else
    r = read(hd_, buf, buflen);
#endif
    return r;
}

//写入
size_t MyTap::Write(const char *buf, size_t buflen){
    size_t r = 0;
    if(buflen==0){
        return r;
    }
#ifdef _WIN32
    DWORD wd = 0;
    if (zs_)
    {
        WriteFile(hd_, buf, buflen, &wd, nullptr);
        r = wd;
    }else{
        OVERLAPPED poa;
        newOverlapped(&poa);
        if(ResetEvent(poa.hEvent)==FALSE){
            cout << "ResetEvent failed" << endl;
            return r;
        }
        WriteFile(hd_, buf, buflen, &wd, &poa);
        r = getOverlappedResult(hd_, &poa);
    }
#else
    r = write(hd_, buf, buflen);
#endif
    return r;
}

MyTap MyTap::NewTAP(const MyMAC *mac, const MyIP *ip, const MyMask *mask)
{
    MyTap tap;
#ifdef _WIN32
    if (tap.SetMac(mac) != 0)
    {
        throw("set mac err");
        return tap;
    }

    tap.SetDisable();
    tap.SetEnable();

    if (tap.Open() != 0)
    {
        throw("open err");
        return tap;
    }

    if (tap.SetIPMask(ip, mask) != 0)
    {
        throw("set ip and mask err");
        return tap;
    }

    if (tap.SetStatus(true) != 0)
    {
        throw("enable err");
        return tap;
    }

    // if (tap.SetMTU(1400) != 0)
    // {
    //     throw("set mtu err");
    //     return tap;
    // }
#else
    if (tap.Open() != 0)
    {
        throw("open err");
        return tap;
    }
    if (tap.SetIPMask(ip, mask) != 0)
    {
        throw("set ip and mask err");
        return tap;
    }
    tap.SetEnable();
    if (tap.SetMac(mac) != 0)
    {
        throw("set mac err");
        return tap;
    }

#endif
    //修改MTU在1400以下,不然会在传输1024字节的时候卡死
    tap.SetMTU(1400);
    return tap;
}

MyTap MyTap::NewTAP(const MyMAC *mac, const MyIPNet *ipnet)
{
    const MyIP &ip = ipnet->GetIP();
    const MyMask &mask = ipnet->GetMask();
    return NewTAP(mac, &ip,&mask);
}

MyTap MyTap::NewTUN(const MyIP *ip,const MyMask *mask)
{
    MyTap tap;
#ifdef _WIN32
    tap.SetDisable();
    tap.SetEnable();

    if (tap.Open() != 0)
    {
        throw("open err");
        return tap;
    }

    if (tap.SetIPMask(ip, mask) != 0)
    {
        throw("set ip and mask err");
        return tap;
    }

    if (tap.SetStatus(true) != 0)
    {
        throw("enable err");
        return tap;
    }
    if(tap.SetTUN()!=0){
        throw("set tap err");
        return tap;
    }
#else
    if (tap.opentun() != 0)
    {
        throw("open err");
        return tap;
    }
    if (tap.SetIPMask(ip, mask) != 0)
    {
        throw("set ip and mask err");
        return tap;
    }
    if (tap.SetTUN() != 0)
    {
        throw("set tap err");
        return tap;
    }
    tap.SetEnable();
#endif
    return tap;
}

MyTap MyTap::NewTUN(const MyIPNet *ipnet)
{
    const MyIP &ip = ipnet->GetIP();
    const MyMask &mask = ipnet->GetMask();
    return NewTUN(&ip, &mask);
}