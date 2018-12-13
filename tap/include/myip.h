//IP类

#if !defined(__H__MYIP__H__)
#define __H__MYIP__H__

#include <string>

class MyIP
{
private:
  unsigned char data[4] = {0};

public:
  MyIP(/* args */);
  ~MyIP();

  //从字符串解析
  int Parse(const std::string &ip);

  //转为字符串
  std::string ToString() const;
};

#endif // __H__MYIP__H__
