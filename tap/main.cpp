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
#include "task.h"
#include "mycompress.h"

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
    // uint64_t outlen = 65535;
    // bs.resize(outlen);
    // MyCompress::DeCompress((unsigned char *)data, len, &bs[0], &outlen);
    // return outlen;
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
    // uint64_t outlen = len + len / 16 + 64 + 3;
    // bs.resize(outlen);
    // MyCompress::Compress((unsigned char *)data, len, &bs[0], &outlen);
    // // {
    // //     cout << "send data:" << endl;
    // //     for (unsigned int i = 0; i < bs.size()&&i<50;++i){
    // //         printf("%02X ", bs[i]);
    // //     }
    // //     printf("\n");
    // //     cout << "src mac" << endl;
    // //     unsigned char out[65535] = {0};
    // //     uint64_t newlen;
    // //     cout << MyCompress::DeCompress(&bs[0], outlen, out, &newlen) << endl;
    // //     for (int i = 6; i < 12; ++i)
    // //     {
    // //         cout << (unsigned)out[i] << " " << flush;
    // //     }
    // //     cout << endl;
    // // }
    // return outlen;
    bs = {data, data + len};
    return bs.size();
}

int th_listen(MyTap* tap){
    static char buf[65535] = {0};
    int r = 0;
    for (;;)
    {
        r = g_mbc->Read(buf, 65534);
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

//从TUN/TAP读取数据
int read_tap(MyTap& tap){
    int r = 0;

    int n = 0;
    // //TUN至少写入20字节,TAP至少写入14字节
    // n = tap.Write("12345678901234567890", 14);
    char buf[65535 + 14] = {0};

    while ((n = tap.Read(buf, 65535)) >= 0)
    {
        // static int c = 0;
        // cout << ++c << " tap read:"<< n << endl;
        if (n < 14)
        {
            continue;
        }

        //不是本机MAC不发送
        auto cfg = Config::GetInstance();
        MyMAC mac(cfg->MAC);
        if(memcmp(buf+6,mac.data,6)!=0){
            continue;
        }

        BinArr bs;
        //加密压缩等操作
        if (handle_data_recv_tap(buf, n, bs) < 0)
        {
            continue;
        }

        //发送
        g_mbc->Write((const char *)&bs[0], bs.size());
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

    read_tap(tap);

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

    read_tap(tap);

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

    // //发送心跳包线程
    // r = DetachHeartBeat(10);
    // if (r < 0)
    // {
    //     return r;
    // }

    auto cfg = Config::GetInstance();
    const auto &sip = cfg->ServerIP;
    auto port = cfg->DataPort;

    MyIP ip(sip);
    while(1){//直到连接成功
        cout << "connect to " << ip.ToString() << ":" << port << flush;
        try
        {
            // g_mbc = new MyBufConn(&ip, port, ConnType::TCP);
            g_mbc = new MyBufConn(&ip, port, ConnType::UDP);
            cout << " success" << endl;
            break;
        }
        catch (...)
        {
            r = -1;
            cout <<" failed,retry after 10 second..." << endl;
            this_thread::sleep_for(chrono::seconds(10));
        }
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
//登录
int login(const MyMAC &mac,const MyIP &ip){
    int r = 0;
    auto cfg = Config::GetInstance();
    MyIP sip(cfg->ServerIP);
    auto port = cfg->CTLPort;
    MySocket ms;
    auto cn = ms.Connect(&sip, port, ConnType::UDP);
    // auto cn = ms.Connect(&ip, 8876, ConnType::TCP,12346);
    int namelen = cfg->UserName.length();
    int pwdlen = cfg->PassWord.length();
    if(namelen>65535||pwdlen>65535){
        cout << "username or password too long" << endl;
        r = -1;
        return r;
    }
    BinArr buf = {0x01};
    //[0x01,mac:6,ip:4,namelen:2,name:namelen,pwdlen:2,pwd:pwdlen]
    buf.insert(buf.end(),mac.data,mac.data+6);
    buf.insert(buf.end(), ip.data, ip.data + 4);
    buf.emplace_back(namelen >> 8);
    buf.emplace_back(namelen & 0x00ff);
    buf.insert(buf.end(),cfg->UserName.cbegin(),cfg->UserName.cend());
    buf.emplace_back(pwdlen >> 8);
    buf.emplace_back(pwdlen & 0x00ff);
    buf.insert(buf.end(), cfg->PassWord.cbegin(), cfg->PassWord.cend());
    r = cn.Write((const char *)&buf[0], buf.size());
    cout << "login ..." << endl;
    // BinArr recbuf(2048);
    char recbuf[2048] = {0};
    cn.Read(recbuf,2048);
    auto code = recbuf[0];
    if (code!= 0)
    {
        return -1;
    }
    auto msg = recbuf[1];
    switch (msg)
    {
    case 1:
        cout << "login success" << endl;
        r = 0;
        break;
    case 2:
        cout << "password or username wrong" << endl;
        r = msg;
        break;
    default:
        break;
    }
    
    return r;
}

int main(int argc, char const *argv[])
{
    int r = 0;

    r=test(argc, argv);
    if(r!=0){
        return r;
    }

    r=init(argc, argv);
    if(r<0){
        return r;
    }

    auto cfg = Config::GetInstance();

    // auto macstr = cfg->MAC;
    // auto netstr = cfg->IPNET;

    MyIPNet ipnet(cfg->IPNET);
    MyMAC mac(cfg->MAC);

    r = login(mac, ipnet.GetIP());
    if (r !=0)
    {
        cout << "login failed" << endl;
        return r;
    }

#ifdef _WIN32
    testwin(&mac,&ipnet);
#else
    testunix(&mac, &ipnet);
#endif

    cleanup();
    return r;
}
