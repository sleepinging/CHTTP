#include "myipnet.h"

MyIPNet::MyIPNet(/* args */)
{
}

MyIPNet::~MyIPNet()
{
}

MyIPNet::MyIPNet(const MyIP &ip, const MyMask &mask)
{
}

MyIPNet::MyIPNet(const std::string &ipnetstr)
{
}

/**
 * @description: 一个IP是否属于这个网络
 * @param {type} 
 * @return: 
 */
bool MyIPNet::Contain(const MyIP &ip){
    return false;
}