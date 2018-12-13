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
  ~MyMAC();

  //从字符串解析
  int Parse(const std::string &mac);
};

#endif // __H__MYMAC__H__

