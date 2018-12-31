//MAC类
#
#if !defined(__H__MYMAC__H__)
#define __H__MYMAC__H__

#include <string>

class MyMAC
{
public:
  unsigned char data[6] = {0};

public:
  MyMAC(/* args */);
  MyMAC(const std::string &mac);
  MyMAC(const MyMAC &mac);
  ~MyMAC();

  //从字符串解析
  int Parse(const std::string &mac);

  //转为大写字符串,加上分隔符
  std::string ToLow(const char chsp=':') const ;

  //转为大写字符串,加上分隔符
  std::string ToUp(const char chsp = ':') const;

  //比较
  bool operator==(const MyMAC &mac) const;

  //赋值
  MyMAC &operator=(const MyMAC &mac);
};

#endif // __H__MYMAC__H__

