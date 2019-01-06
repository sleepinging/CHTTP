#include <stdio.h>
#include <errno.h>
#include <signal.h>

#include <string>
#include <iostream>
#include <thread>

#ifdef _WIN32
#include <WinSock2.h>
#include <Windows.h>
#include "tap-windows.h"
#endif

#include "mytap.h"
#include "myip.h"
#include "mymac.h"
#include "mymask.h"
#include "mytool.h"
#include "mysocket.h"
#include "netdef.hpp"
#include "mybufconn.h"
#include "channel.hpp"
#include "cmdline.h"
#include "myipnet.h"
#include "test.h"
#include "config.h"

using namespace std;

static MyBufConn* g_mbc = nullptr;

/**
 * @description: 处理从服务器接收到的数据,解压|解密
 * @param 
 *  data:数据
 *  len:数据长度
 *  bs:处理之后的数据
 * @return: 返回小于0表示无效数据
 */
int handle_data_recv_remote(const char* data,size_t len,BinArr& bs){
    bs = {data, data + len};
    return bs.size();
}

/**
 * @description: 处理从tap设备读取的数据,加密|压缩
 * @param 
 *  data:数据
 *  len:数据长度
 *  bs:处理之后的数据
 * @return: 返回小于0表示无效数据
 */
int handle_data_recv_tap(const char *data, size_t len, BinArr &bs)
{
    bs = {data, data + len};
    return bs.size();
}

int th_listen(MyTap* tap){
    static char buf[2048] = {0};
    int r = 0;
    for (;;)
    {
        r = g_mbc->Read(buf, 2047);
        if(r<=0){
            continue;
        }
        // buf[r] = 0;
        // cout<<"socket read:"<<r<<endl;
        // cout<<"socket read:"<<r<<" wait for write tap ..." <<endl;
        BinArr bs;
        if(handle_data_recv_remote(buf, r, bs)<0){
            continue;
        }
        r = tap->Write((const char*)&bs[0], bs.size());
        // cout << "tap writed "<<r << endl;
    }

    return r;
}

#ifdef _WIN32
int testwin(const MyMAC *mac, const MyIPNet* ipnet)
{
    int r = 0;

    auto tap = MyTap::NewTAP(mac, ipnet);
    thread(th_listen,&tap).detach();
    // for(;;this_thread::sleep_for(chrono::seconds(1)));

    int n = 0;
    // //TUN至少写入20字节,TAP至少写入14字节
    // n = tap.Write("12345678901234567890", 14);
    char buf[2048 + 14] = {0};

    while((n = tap.Read(buf, 2048))>=0){
        // static int c = 0;
        // cout << ++c << " tap read:"<< n << endl;
        if(n<=0){
            continue;
        }

        BinArr bs;
        if(handle_data_recv_tap(buf, n, bs)<0){
            continue;
        }

        g_mbc->Write((const char*)&bs[0], bs.size());
    }

    //tap.SetDisable();

    if (tap.SetStatus(false) != 0)
    {
        showerr("disable err");
        return -1;
    }
    if (tap.Close() != 0)
    {
        showerr("close err");
        return -1;
    }
    return r;
}
#else
int testunix(const MyMAC *mac, const MyIPNet *ipnet)
{
    int r = 0;

    signal(SIGPIPE, SIG_IGN);

    auto tap = MyTap::NewTAP(mac, ipnet);
    // auto tap = MyTap::NewTUN(ipnet);
    thread(th_listen,&tap).detach();

    int n = 0;
    // //TUN至少写入20字节,TAP至少写入14字节
    // n = tap.Write("12345678901234567890", 14);
    unsigned char buf[2048 + 14] = {0};

    while ((n = tap.Read((char *)buf, 2048)) >= 0)
    {
        // static int c = 0;
        // cout << ++c << " tap read:"<< n << endl;

        g_mbc->Write((const char *)buf, n);
    }
    return r;
}
#endif

int init(int argc, char const *argv[])
{
    // //去除警告
    // argc = argc, argv = argv;

    int r = 0;
    r = MySocket::InitLib();

    cmdline::parser a;
    a.add<string>("config", 'c', "config file", false, "config.ini");
    a.parse_check(argc, argv);
    auto cfgfile = a.get<string>("config");

    r = Config::Init(cfgfile) ? 0 : -1;
    if(r<0){
        return r;
    }

    auto cfg = Config::GetInstance();
    const auto &sip = cfg->ServerIP;
    auto port = cfg->ServerPort;

    MyIP ip(sip);
    try{
        g_mbc = new MyBufConn(&ip, port, ConnType::TCP);
    }catch(...){
        r = -1;
    }

    return r;
}

int cleanup(){
    int r=0;
    delete g_mbc;
    g_mbc = nullptr;
    r = MySocket::CleanLib();
    return r;
}

int main(int argc, char const *argv[])
{
    int r = 0;
    // r=test(argc, argv);
    // return r;

    r=init(argc, argv);
    if(r<0){
        return r;
    }

    auto cfg = Config::GetInstance();

    // auto macstr = cfg->MAC;
    // auto netstr = cfg->IPNET;

    MyIPNet ipnet(cfg->IPNET);
    MyMAC mac(cfg->MAC);

#ifdef _WIN32
    testwin(&mac,&ipnet);
#else
    testunix(&mac, &ipnet);
#endif

    cleanup();
    return r;
}
