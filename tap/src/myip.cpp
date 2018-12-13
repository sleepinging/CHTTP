#include "myip.h"

#include <stdio.h>

#include "mytool.h"

using namespace std;

MyIP::MyIP(/* args */)
{
}

MyIP::~MyIP()
{
}

//从字符串解析
int MyIP::Parse(const std::string &ip){
    int iplen = ip.length();
    if (iplen>15||iplen<7){
        return -1;
    }
    return 0;
}

//转为字符串
std::string MyIP::ToString() const{

}