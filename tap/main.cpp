#include <string>

#include <Windows.h>

#include "tap-windows.h"

#include "mytap.h"

using namespace std;

void showerr(const char* msg=""){
    char error[1000];
    sprintf(error, "%s %lu",msg, GetLastError());
    MessageBoxA(0, error, "error", 0);
}

int main(int argc, char const *argv[])
{
    MyTap tap;
    if(tap.Open()==-1){
        showerr("open err");
        return -1;
    }
    if(tap.SetStatus(true)==0){
        showerr("control err");
        return -1;
    }
    return 0;
}
