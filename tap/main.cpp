#include <stdio.h>
#include <errno.h>

#include <string>
#include <iostream>

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
#include "packet.h"
#include "mysocket.h"
#include "netdef.hpp"

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

#ifdef _WIN32
int testwin()
{
    int r = 0;

    MyMAC mac("01:02:03:04:05:07");
    MyIP ip("192.168.10.100");
    MyMask mask(24);

    auto tap = MyTap::NewTAP(&mac,&ip,&mask);

    int n = 0;
    // //TUN至少写入20字节,TAP至少写入14字节
    // n = tap.Write("12345678901234567890", 14);
    unsigned char buf[2048 + 14] = {0};

    while((n = tap.Read((char*)buf, 2048))>=0){
        static int c = 0;
        cout << ++c << " read:" << n << endl;

        Packet* p = new Packet();
        if(0==p->Parse(buf, n)){
            p->print();
        }

        // for (int i = 0; i < n;++i){
        //     putchar(buf[i]);
        // }
        // cout << endl;
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
int testunix(){
    int r = 0;

    MyMAC mac("00:01:23:45:67:89");
    MyIP ip("192.168.10.110");
    MyMask mask(24);

    auto tap = MyTap::NewTAP(&mac, &ip, &mask);
    // auto tap = MyTap::NewTUN(&ip, &mask);

    int n = 0;
    // //TUN至少写入20字节,TAP至少写入14字节
    // n = tap.Write("12345678901234567890", 14);
    unsigned char buf[2048 + 14] = {0};

    while ((n = tap.Read((char *)buf, 2048)) >= 0)
    {
        static int c = 0;
        cout << ++c << " read:" << n << endl;

        Packet *p = new Packet();
        if (0 == p->Parse(buf, n))
        {
            p->print();
        }

        // for (int i = 0; i < n;++i){
        //     putchar(buf[i]);
        // }
        // cout << endl;
    }
    return r;
}
#endif

int init(){
    int r = 0;
    r = MySocket::InitLib();
    return r;
}

int cleanup(){
    int r=0;
    r = MySocket::CleanLib();
    return r;
}

int connecttest(){
    MySocket ms;
    MyIP ip("192.168.1.100");
    auto cn = ms.Connect(&ip, 8875, ConnType::UDP);
    // auto cn = ms.Connect(&ip, 8876, ConnType::TCP,12346);
    int r = cn.Write("123", 3);
    r = cn.Write("456", 3);
    r = cn.Write("798", 3);
    if(r<0){
        showerr("write");
    }
    // char buf[2048]={0};
    // r = cn.Read(buf, 2047);
    // cout << string(buf, r) << endl;

    return r;
}

int main(/*int argc, char const *argv[]*/)
{
    init();

    // connecttest();

#ifdef _WIN32
    testwin();
#else
    testunix();
#endif

    cleanup();
    return 0;
}
