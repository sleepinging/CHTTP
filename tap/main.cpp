#include <string>

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
    MyTap tap;
    if (tap.Open() == -1)
    {
        showerr("open err");
        return -1;
    }
    if (tap.SetStatus(true) == 0)
    {
        showerr("control err");
        return -1;
    }
    if (tap.Close() == 0)
    {
        showerr("close err");
        return -1;
    }
    return 0;
}

int main(/*int argc, char const *argv[]*/)
{
    MyMask mask(21);
    unsigned char buf[4];
    mask.ToIPv4Mask(buf);
    MyIP ip,net;
    ip.Parse("192.168.9.138");
    mask.ToNetIPv4(&ip, &net);
    return 0;
}
