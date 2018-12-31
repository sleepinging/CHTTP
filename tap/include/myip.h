//IP类

#if !defined(__H__MYIP__H__)
#define __H__MYIP__H__

#include <string>

class MyIP
{
public:
  unsigned char data[4] = {0};

public:
  MyIP(/* args */);
  MyIP(const std::string &ip);
  ~MyIP();
  
  //复制
  MyIP(const MyIP &ip);

  //复制
  MyIP(MyIP &&ip);

  //从字符串解析
  int Parse(const std::string &ip);

  //转为字符串
  std::string ToString() const;

  //比较
  bool operator==(const MyIP &ip) const;

  //赋值
  MyIP &operator=(const MyIP &ip);

};

#endif // __H__MYIP__H__
