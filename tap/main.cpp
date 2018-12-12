#include <stdio.h>
#include <malloc.h>

#include <string>

#include <Windows.h>

#include "tap-windows.h"

using namespace std;

std::string GetComponentId()
{
    char szOwnerKeyPath[] = ADAPTER_KEY;
    HKEY hOwnerKey = NULL; // {4d36e972-e325-11ce-bfc1-08002be10318}：类别：NSIS网卡驱动
    string szDevComponentId ="";
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, szOwnerKeyPath, 0, KEY_ALL_ACCESS, &hOwnerKey) == ERROR_SUCCESS)
    {
        char szClassName[MAX_PATH];
        DWORD dwIndex = 0;
        while (szDevComponentId == "" && RegEnumKeyA(hOwnerKey, dwIndex++, szClassName, MAX_PATH) == ERROR_SUCCESS)
        {
            BYTE data[MAX_PATH];
            DWORD dwRegType = REG_NONE;
            DWORD dwSize = sizeof(data);
            HKEY hSubKey = NULL;
            //char szSubKeyPath[MAX_PATH];
            string szSubKeyPath=string(szOwnerKeyPath)+"\\"+szClassName;
            //sprintf(szSubKeyPath, "%s\\%s", szOwnerKeyPath, szClassName);
            if (RegOpenKeyA(HKEY_LOCAL_MACHINE, szSubKeyPath.c_str(), &hSubKey) != ERROR_SUCCESS)
            {
                continue;
            }
            if (RegQueryValueExA(hSubKey, "ComponentId", NULL, &dwRegType, data, &dwSize) == ERROR_SUCCESS && dwRegType == REG_SZ)
            {
                dwSize = sizeof(data);
                if (strcmp("tap0901", (char*)data) == 0 && RegQueryValueExA(hSubKey, "NetCfgInstanceId", NULL,
                    &dwRegType, data, &dwSize) == ERROR_SUCCESS && dwRegType == REG_SZ)
                {
                    //szDevComponentId = (char*)memcpy(malloc(dwSize), data, dwSize);
                    szDevComponentId=(const char*)data;
                }
            }
            RegCloseKey(hSubKey);
        }
        RegCloseKey(hOwnerKey);
    }
    return szDevComponentId;
}

void showerr(const char* msg=""){
    char error[1000];
    sprintf(error, "%s %lu",msg, GetLastError());
    MessageBoxA(0, error, "error", 0);
}

HANDLE open(){
    auto id = GetComponentId();
    string strDriver = "\\\\.\\Global\\";
    //strDriver = "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002BE10318}";
    strDriver += id + TAP_WIN_SUFFIX;
    //{xxxxx-xxxxx-xxxxx-xxxxx-xxxxx}改成你的网卡的名称
    //即HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\NetworkCards键下某网卡的ServiceName值

    HANDLE hICDriver = CreateFileA(strDriver.c_str(),
                                   GENERIC_READ | GENERIC_WRITE,
                                   FILE_SHARE_READ | FILE_SHARE_WRITE,
                                   NULL,
                                   OPEN_EXISTING,
                                   FILE_ATTRIBUTE_NORMAL,
                                   NULL);
    return hICDriver;
}

int setStatus(HANDLE hICDriver,bool st)
{
    char code[4] = {0};
    code[0] = st ? 1 : 0;
    char buf[2048] = {0};
    DWORD n;
    return DeviceIoControl(hICDriver, TAP_WIN_IOCTL_SET_MEDIA_STATUS, code, 4, buf, 2048, &n, nullptr);
}

int main(int argc, char const *argv[])
{
    auto hd = open();
    if (hd == INVALID_HANDLE_VALUE)
    {
        showerr("can't open");
        return -1;
    }

    auto r = setStatus(hd,true);
    if (r == 0)
    {
        showerr("can't control");
        return -1;
    }
    // char buffer[10];
    // DWORD dwLen;
    // BOOL bRet=WriteFile(hICDriver,buffer,10,&dwLen,NULL);
    // if(bRet==FALSE)
    // {
    //     showerr("cannot write");
    // }
    
    CloseHandle(hd);
    return 0;
}
