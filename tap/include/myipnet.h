/*
 * @Description: IP相关类
 * @Author: taowentao
 * @Date: 2018-12-31 10:32:07
 * @LastEditors: taowentao
 * @LastEditTime: 2018-12-31 14:20:12
 */

#include <string>

#include "myip.h"
#include "mymask.h"

class MyIPNet
{
private:
  //192.168.10.111
  MyIP ip_;
  //192.168.10.0
  MyIP netip_;
  //24
  MyMask mask_;
  //255.255.255.0
  MyIP maskip_;

public:
  MyIPNet(/* args */);
  MyIPNet(const MyIP &ip,const MyMask& mask);
  MyIPNet(MyIP &&ip,MyMask&& mask);
  MyIPNet(const std::string &ipnetstr);
  ~MyIPNet();

public:
  const MyMask &GetMask() const;

  const MyIP &GetIP() const;

public:
/**
 * @description: 一个IP是否属于这个网络
 * @param {type} 
 * @return: 
 */
  bool Contain(const MyIP &ip) const;

/**
 * @description: 转字符串 192.168.10.0/24
 * @param {type} 
 * @return: 
 */
  std::string ToString() const;
};
