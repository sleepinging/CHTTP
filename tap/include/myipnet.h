/*
 * @Description: IP相关类
 * @Author: taowentao
 * @Date: 2018-12-31 10:32:07
 * @LastEditors: taowentao
 * @LastEditTime: 2018-12-31 10:44:08
 */

#include <string>

#include "myip.h"
#include "mymask.h"

class MyIPNet
{
private:
  MyIP ip_;
  MyMask mask_;
  MyIP maskip_;

public:
  MyIPNet(/* args */);
  MyIPNet(const MyIP &ip,const MyMask& mask);
  MyIPNet(MyIP &&ip,MyMask&& mask);
  MyIPNet(const std::string &ipnetstr);
  ~MyIPNet();
public:
/**
 * @description: 一个IP是否属于这个网络
 * @param {type} 
 * @return: 
 */
  bool Contain(const MyIP &ip);
};
