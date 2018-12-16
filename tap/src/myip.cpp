#include "myip.h"

#include <stdio.h>
#include <string.h>

#include <vector>

#include "mystring.h"

using namespace std;

MyIP::MyIP(/* args */)
{
}

MyIP::~MyIP()
{
}

//复制
MyIP::MyIP(const MyIP &ip)
{
    memcpy(this->data, ip.data, 4);
}

MyIP::MyIP(const std::string &ip)
{
    if(Parse(ip)==-1){
        throw "ip err";
    }
}

//从字符串解析
int MyIP::Parse(const std::string &ip){
    int iplen = ip.length();
    if (iplen>15||iplen<7){
        return -1;
    }
    vector<string> v;
    split(ip, ".", v);
    if(v.size()!=4){
        return -1;
    }
    unsigned char s = 0;
    for (int i = 0; i < 4; ++i)
    {
        s = 0;
        const auto &d = v[i]; //192
        for(const auto c:d){//1 9 2
            s *= 10;
            s += (c - '0');
        }
        data[i] = s;
    }
    return 0;
}

//转为字符串
std::string MyIP::ToString() const{
    char buf[15] = {0};
    sprintf(buf, "%d.%d.%d.%d", data[0], data[1], data[2], data[3]);
    return buf;
}

//比较
bool MyIP::operator==(const MyIP &ip) const
{
    return memcmp(this->data, ip.data, 4) == 0;
}