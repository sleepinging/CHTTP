#include "myipnet.h"

#include <iostream>

#include "mystring.h"

using namespace std;

MyIPNet::MyIPNet(/* args */)
{
}

MyIPNet::~MyIPNet()
{
}

MyIPNet::MyIPNet(const MyIP &ip, const MyMask &mask)
    :ip_(ip),mask_(mask)
{
    mask_.ToIPv4Mask(maskip_.data);
    mask_.ToNetIPv4(&ip, &netip_);
}

MyIPNet::MyIPNet(MyIP &&ip, MyMask &&mask)
    :ip_(std::move(ip)),mask_(std::move(mask))
{
    mask_.ToIPv4Mask(maskip_.data);
    mask_.ToNetIPv4(&ip, &netip_);
}

MyIPNet::MyIPNet(const std::string &ipnetstr)
{
    auto idx = ipnetstr.find("/");
    if(idx==string::npos||idx==ipnetstr.length()-1){
        cout << "wrong ipnet str" << endl;
        throw "wrong ipnet str";
    }
    auto ipstr = string(ipnetstr.cbegin(), ipnetstr.begin() + idx);
    auto maskstr = string(ipnetstr.cbegin() + idx+1, ipnetstr.cend());
    ip_.Parse(ipstr);
    unsigned char mv = mytrans<string, int>(maskstr);
    if(mv>32){
        cout << "wrong mask str" << endl;
        throw "wrong mask str";
    }
    mask_.value = mv;
    mask_.ToIPv4Mask(maskip_.data);
    mask_.ToNetIPv4(&ip_, &netip_);
}

const MyMask &MyIPNet::GetMask() const
{
    return mask_;
}

const MyIP &MyIPNet::GetIP() const
{
    return ip_;
}

/**
 * @description: 一个IP是否属于这个网络
 * @param {type} 
 * @return: 
 */
bool MyIPNet::Contain(const MyIP &ip) const{
    for (int i=0; i < 4; ++i)
    {
        if((ip.data[i]&maskip_.data[i])!=netip_.data[i]){
            return false;
        }
    }
    return true;
}

/**
 * @description: 转字符串 192.168.10.0/24
 * @param {type} 
 * @return: 
 */
std::string MyIPNet::ToString() const{
    return ip_.ToString() + "/" + mytrans<int,string>(mask_.value);
}