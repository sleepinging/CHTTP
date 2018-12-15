#include "mymac.h"

#include "mystring.h"

using namespace std;

MyMAC::MyMAC(/* args */)
{
}

MyMAC::MyMAC(const std::string &mac)
{
    if(Parse(mac)==-1){
        throw "mac err";
    }
}

MyMAC::~MyMAC()
{
}

//从字符串解析
int MyMAC::Parse(const std::string &mac)
{
    if(mac.length()<12){
        return -1;
    }
    if(mac.length()==12){
        for (int i = 0; i < 6;++i){
            data[i] = atohex(mac[i*2]) * 0x10 + atohex(mac[2*i + 1]);
        }
    }
    if (mac.length() == 17){
        for (int i = 0; i < 6; ++i)
        {
            data[i] = atohex(mac[i*3]) * 0x10 + atohex(mac[i*3 + 1]);
        }
    }
    return 0;
}

//转为大写字符串,加上分隔符
std::string MyMAC::ToLow(const char chsp) const
{
    const int splen = 1;
    const int buflen = 12 + 5 * 1;
    string mac;
    mac.resize(buflen);
    for (int i = 0; i < 5;++i){
        hextoa(data[i], &mac[i * 2 + i * splen]);
        mac[i * 2 + i * splen + 2] = chsp;
    }
    hextoa(data[5], &mac[10+5*splen]);
    return mac;
}

//转为大写字符串,加上分隔符
std::string MyMAC::ToUp(const char chsp) const
{
    string mac;
    int splen = chsp==0?0:1;
    int buflen = 12 + 5 * splen;
    mac.resize(buflen);
    for (int i = 0; i < 5; ++i)
    {
        hextoA(data[i], &mac[i * 2 + i * splen]);
        mac[i * 2 + i * splen + 2] = chsp;
    }
    hextoA(data[5], &mac[10 + 5 * splen]);
    return mac;
}