#include <string>

#include <iostream>

#include <Windows.h>

#include "tap-windows.h"

#include "mytap.h"
#include "myip.h"
#include "mymac.h"
#include "mymask.h"
#include "mytool.h"

using namespace std;

void showerr(const char* msg=""){
    char error[1000];
    sprintf(error, "%s %lu",msg, GetLastError());
    MessageBoxA(0, error, "error", 0);
}

int test(){
    int r = 0;
    MyTap tap;
    if (tap.Open() !=0 )
    {
        showerr("open err");
        return -1;
    }
    if (tap.SetStatus(true) != 0)
    {
        showerr("enable err");
        return -1;
    }

    tap.SetEnable();

    MyIP ip("192.168.9.138");
    MyMask mask(21);
    tap.SetIPMask(&ip,&mask);

    MyMAC mac("94:68:11:22:33:44");
    if(tap.SetMac(&mac)!=0){
        showerr("set mac err");
        return -1;
    }

    if (tap.SetMTU(1400) != 0)
    {
        showerr("set mtu err");
        return -1;
    }

    // r = tap.SetTAP();
    // auto n = tap.Write("123456", 6);
    // char buf[2048] = {0};
    // n = tap.Read(buf, 2048);

    // cout << "read:" << buf << endl;

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
