#include <stdio.h>

#include <string>
#include <iostream>

#include <WinSock2.h>
#include <Windows.h>

#include "tap-windows.h"

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
    char error[1000];
    sprintf(error, "%s %lu",msg, GetLastError());
    MessageBoxA(0, error, "error", 0);
}

int test(){
    int r = 0;
    MyTap tap;

    MyMAC mac("01:02:03:04:05:07");
    if(tap.SetMac(&mac)!=0){
        showerr("set mac err");
        return -1;
    }

    tap.SetDisable();
    tap.SetEnable();

    if (tap.Open() != 0)
    {
        showerr("open err");
        return -1;
    }

    MyIP ip("192.168.10.100");
    MyMask mask(24);
    if(tap.SetIPMask(&ip, &mask)!=0){
        showerr("set ip and mask err");
        return -1;
    }

    if (tap.SetStatus(true) != 0)
    {
        showerr("enable err");
        return -1;
    }

    if (tap.SetMTU(1400) != 0)
    {
        showerr("set mtu err");
        return -1;
    }

    r = tap.SetTAP();

    int n = 0;
    // //至少写入20字节
    // n = tap.Write("12345678901234567890", 20);
    unsigned char bufarr[2048 + 14] = {0};
    memcpy(bufarr, mac.data, 6);
    memcpy(bufarr+6, mac.data, 6);
    bufarr[12] = 0x08;
    bufarr[13] = 0x00;
    unsigned char *buf = bufarr + 14;

    while((n = tap.Read((char*)buf, 2048))>=0){
        static int c = 0;
        cout << ++c << " read:" << n << endl;

        Packet* p = new Packet();
        if(0==p->Parse(bufarr, n)){
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

int main(/*int argc, char const *argv[]*/)
{
    init();
    
    test();

    cleanup();
    return 0;
}
