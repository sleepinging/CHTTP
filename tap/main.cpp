#include <stdio.h>

#include <string>
#include <iostream>

#include <Windows.h>

#include "tap-windows.h"

#include "mytap.h"
#include "myip.h"
#include "mymac.h"
#include "mymask.h"
#include "mytool.h"
#include "mypacket.h"

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
    //n = tap.Write("123456", 6);
    unsigned char buf[2048] = {0};

    while((n = tap.Read((char*)buf, 2048))>=0){
        // if(buf[12]!=0x08||buf[13]!=0x00){
        //     continue;
        // }
        cout << "read:" << endl;
        for (int i = 0; i < n;++i){
            putchar(buf[i]);
        }
        cout << endl;
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

int main(/*int argc, char const *argv[]*/)
{
    //reg();
    test();
    return 0;
}
