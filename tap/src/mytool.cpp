#include "mytool.h"

#include <stdlib.h>

#include <iostream>

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

//获取注册表项的值
std::string GetRegValue(const std::string &dir, const std::string name){
    std::string strValue("");
    HKEY hKey = HKEY_LOCAL_MACHINE;//这里暂时先用这个
    HKEY hKeyResult = NULL;
    DWORD dwSize = 0;
    DWORD dwDataType = 0;
    //打开注册表
    if (ERROR_SUCCESS == ::RegOpenKeyExA(hKey, dir.c_str(), 0, KEY_QUERY_VALUE, &hKeyResult))
    {
        // 获取缓存的长度dwSize及类型dwDataType
        ::RegQueryValueExA(hKeyResult, name.c_str(), 0, &dwDataType, NULL, &dwSize);
        switch (dwDataType)
        {
        // case REG_MULTI_SZ://多字符串，多个值
        // {
        //     //分配内存大小
        //     BYTE *lpValue = new BYTE[dwSize];
        //     //获取注册表中指定的键所对应的值
        //     LONG lRet = ::RegQueryValueExA(hKeyResult, name.c_str(), 0, &dwDataType, lpValue, &dwSize);
        //     delete[] lpValue;
        //     break;
        // }
        case REG_SZ:
        {
            //分配内存大小
            char *lpValue = new char[dwSize];
            memset(lpValue, 0, dwSize * sizeof(char));
            //获取注册表中指定的键所对应的值
            if (ERROR_SUCCESS == ::RegQueryValueExA(hKeyResult, name.c_str(), 0, &dwDataType, (LPBYTE)lpValue, &dwSize))
            {
                //std::wstring wstrValue(lpValue);
                strValue = lpValue;
            }
            delete[] lpValue;
            break;
        }
        default:
            break;
        }
    }

    //关闭注册表
    ::RegCloseKey(hKeyResult);
    return strValue;
}

//执行系统命令
int ExecCmd(std::initializer_list<std::string> cmds, bool show)
{
    if (cmds.size()==0){
        return 0;
    }
    string cmd("");
    for (auto it=cmds.begin(); it<cmds.end()-1;++it){
        cmd += *it + " ";
    }
    cmd +=*(cmds.end() - 1);
    if(!show){
        cmd += " > NUL";
    }else{
        cout << cmd << endl;
    }
    return system(cmd.c_str());
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

//将1个字符转为数字(10进制)
unsigned char atodec(char c){
    return c - '0';
}

//将一个字节转为2个字符(16进制小写)
void hextoa(unsigned char h, char buf[2])
{
    unsigned char a = h >> 4;
    unsigned char b = h & 0x0f;

    if(a<10){
        buf[0] = '0' + a;
    }else{
        buf[0] = 'a' + a-10;
    }

    if (b < 10)
    {
        buf[1] = '0' + b;
    }
    else
    {
        buf[1] = 'a' + b-10;
    }

    return;
}

//将一个字节转为2个字符(16进制大写)
void hextoA(unsigned char h, char buf[2])
{
    unsigned char a = h >> 4;
    unsigned char b = h & 0x0f;

    if (a < 10)
    {
        buf[0] = '0' + a;
    }
    else
    {
        buf[0] = 'A' + a-10;
    }

    if (b < 10)
    {
        buf[1] = '0' + b;
    }
    else
    {
        buf[1] = 'A' + b-10;
    }

    return;
}

void split(const string &s, const string &c, vector<std::string> &v)
{
    string::size_type pos1, pos2;
    pos1 = 0;
    pos2 = s.find(c);
    while (string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2 - pos1));
        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if (pos1 != s.length())
        v.push_back(s.substr(pos1));
}

std::string &trim(std::string &s)
{
    if (s.empty())
        return s;
    s.erase(0, s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}