#include "mytool.h"

#include <Windows.h>

#include "tap-windows.h"

using namespace std;

std::string GetTAPComponentId()
{
    char szOwnerKeyPath[] = ADAPTER_KEY;
    HKEY hOwnerKey = NULL; // {4d36e972-e325-11ce-bfc1-08002be10318}：类别：NSIS网卡驱动
    string szDevComponentId = "";
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
            string szSubKeyPath = string(szOwnerKeyPath) + "\\" + szClassName;
            //sprintf(szSubKeyPath, "%s\\%s", szOwnerKeyPath, szClassName);
            if (RegOpenKeyA(HKEY_LOCAL_MACHINE, szSubKeyPath.c_str(), &hSubKey) != ERROR_SUCCESS)
            {
                continue;
            }
            if (RegQueryValueExA(hSubKey, "ComponentId", NULL, &dwRegType, data, &dwSize) == ERROR_SUCCESS && dwRegType == REG_SZ)
            {
                dwSize = sizeof(data);
                if (strcmp("tap0901", (char *)data) == 0 && RegQueryValueExA(hSubKey, "NetCfgInstanceId", NULL, &dwRegType, data, &dwSize) == ERROR_SUCCESS && dwRegType == REG_SZ)
                {
                    //szDevComponentId = (char*)memcpy(malloc(dwSize), data, dwSize);
                    szDevComponentId = (const char *)data;
                }
            }
            RegCloseKey(hSubKey);
        }
        RegCloseKey(hOwnerKey);
    }
    return szDevComponentId;
}

//将1个字符转为数字(16进制)
unsigned char atohex(char c)
{
    unsigned char s=0;
    if (c >= '0' && c <= '9')
    {
        s = c - '0';
    }else if(c>='a'&&c<='z'){
        s = c - 'a' + 10;
    }
    else if (c >= 'A' && c <= 'Z')
    {
        s = c - 'A' + 10;
    }
    return s;
}