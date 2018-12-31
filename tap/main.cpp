#include <stdio.h>
#include <errno.h>

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

using namespace std;

void showerr(const char* msg=""){
#ifdef _WIN32
    char error[1000];
    sprintf(error, "%s %lu",msg, GetLastError());
    MessageBoxA(0, error, "error", 0);
#else
    // cout << "error:"<<msg << endl;
    perror(msg);
#endif
}

static MyBufConn* g_mbc = nullptr;

int th_listen(MyTap* tap){
    static char buf[2048] = {0};
    int r = 0;
    for (;;)
    {
        r = g_mbc->Read(buf, 2047);
        buf[r] = 0;
        // cout<<"socket read:"<<r<<" wait for write tap ..." <<endl;
        r=tap->Write(buf, r);
        // cout << "tap writed "<<r << endl;
    }

    return r;
}

#ifdef _WIN32
int testwin(const MyMAC &mac, const MyIP &ip, const MyMask &mask)
{
    int r = 0;

    auto tap = MyTap::NewTAP(&mac, &ip, &mask);
    thread(th_listen,&tap).detach();
    // for(;;this_thread::sleep_for(chrono::seconds(1)));

    int n = 0;
    // //TUN至少写入20字节,TAP至少写入14字节
    // n = tap.Write("12345678901234567890", 14);
    unsigned char buf[2048 + 14] = {0};

    while((n = tap.Read((char*)buf, 2048))>=0){
        // static int c = 0;
        // cout << ++c << " tap read:"<< n << endl;

        g_mbc->Write((const char*)buf, n);
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
int testunix(const MyMAC &mac, const MyIP &ip, const MyMask& mask)
{
    int r = 0;

    auto tap = MyTap::NewTAP(&mac, &ip, &mask);
    // auto tap = MyTap::NewTUN(&ip, &mask);
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

int init(){
    int r = 0;
    r = MySocket::InitLib();
    MyIP ip("123.207.91.166");
    try{
        g_mbc = new MyBufConn(&ip, 6543, ConnType::TCP);
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

int connecttest(){
    MySocket ms;
    MyIP ip("192.168.1.100");
    auto cn = ms.Connect(&ip, 8875, ConnType::TCP);
    // auto cn = ms.Connect(&ip, 8876, ConnType::TCP,12346);
    int r = cn.Write("123", 3);
    r = cn.Write("456", 3);
    r = cn.Write("798", 3);
    if(r<0){
        showerr("write");
    }
    char buf[2048]={0};
    while(1){
        r = cn.Read(buf, 2047);
        cout << string(buf, r) << endl;
    }
    

    // MyIP ip("192.168.1.100");
    // MyBufConn bc(&ip, 8765, ConnType::TCP);
    // int r = bc.Write("123", 3);
    // r = bc.Write("456", 3);
    // r = bc.Write("798", 3);

    // char buf[2048]={0};
    // r = bc.Read(buf, 2047);
    // cout << string(buf, r) << endl;

    // if (r < 0)
    // {
    //     showerr("write");
    // }

    // this_thread::sleep_for(chrono::seconds(1));
    return r;
}

int main(int argc, char const *argv[])
{
    MyIPNet in("192.168.8.110/21");
    auto f = in.Contain(MyIP("192.168.15.127"));

    init();

    cmdline::parser a;
    a.add<string>("net", 'n', "ip string", true, "192.168.10.100/24");
    a.add<string>("mac", 'm', "mac string", true, "00:02:03:04:05:07");
    a.parse_check(argc, argv);

    auto netstr = a.get<string>("net");
    auto macstr = a.get<string>("mac");

    MyIPNet ipnet(netstr);
    MyMAC mac(macstr);

    // connecttest();
#ifdef _WIN32
    testwin(a.get<string>("mac"), a.get<string>("ip"), a.get<int>("mask"));
#else
    testunix(a.get<string>("mac"), a.get<string>("ip"), a.get<int>("mask"));
#endif

    cleanup();
    return 0;
}
