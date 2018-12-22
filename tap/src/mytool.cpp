#include "mytool.h"

#include <stdlib.h>
#include <time.h>

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

//修改注册表的值
int SetRegValueString(const std::string &dir, const std::string &name, const std::string &value){
    std::string strValue("");
    HKEY hKey = HKEY_LOCAL_MACHINE; //这里暂时先用这个
    HKEY hKeyResult = NULL;
    //打开注册表
    if (ERROR_SUCCESS != ::RegOpenKeyExA(hKey, dir.c_str(), 0, KEY_ALL_ACCESS, &hKeyResult))
    {
        return -1;
    }
    DWORD dwIndex = 0;
    const DWORD chlen = MAX_PATH-1;   //子键名称长度
    char chname[chlen+1] = {0};       //子键名称
    while (RegEnumKeyA(hKeyResult, dwIndex++, chname, chlen) == ERROR_SUCCESS) //枚举子键
    { 
        HKEY hk;
        //打开当前子键
        string cd = dir + "\\" + chname;//目前子键路径
        //cout<<"dir:" << cd << endl;
        if (ERROR_SUCCESS != ::RegOpenKeyExA(hKey, cd.c_str(), 0, KEY_ALL_ACCESS, &hk))
        {
            continue;
        }
        //枚举键值对
        DWORD idx = 0;
        char kbuf[2048] = {0}; //key名称
        DWORD klen = 2048-1;//key长度
        DWORD tp = REG_SZ;
        BYTE vbuf[2048] = {0}; //值名称
        DWORD vlen = 2048-1;     //值长度
        // auto r = RegEnumValueA(hk, idx++, kbuf, &klen, 0, &tp, vbuf, &vlen);
        // if(r==ERROR_MORE_DATA){
        //     continue;
        // }
        while (RegEnumValueA(hk, idx++, kbuf, &klen, 0, &tp, vbuf, &vlen) == ERROR_SUCCESS) //枚举键值对
        {
            //cout << kbuf << endl;
            if (klen < name.length())
            {
                continue;
            }
            if (memcmp(kbuf, name.c_str(), name.length()) == 0)
            {
                if (RegSetValueExA(hk, kbuf,0, REG_SZ, (const BYTE*)value.c_str(), value.length()) == ERROR_SUCCESS)
                {
                    ::RegCloseKey(hk);
                    ::RegCloseKey(hKeyResult);
                    return 0;
                }
            }
            memset(kbuf, 0, 2047);
            klen = 2047;
            memset(vbuf, 0, 2047);
            vlen = 2047;
        }
    }
    ::RegCloseKey(hKeyResult);
    return -1;
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

namespace MyTime{
//时间格式化
std::string fmttime(time_t t, const std::string &fstr)
{
    char buf[64] = {0};
    strftime(buf, 63, fstr.c_str(), localtime(&t));
    return string(buf);
}
}
