//加解密
#if !defined(__H__MYCRYPTO__H__)
#define __H__MYCRYPTO__H__

#include "mydef.hpp"

class MyCrypto
{
private:
    /* data */
public:
    MyCrypto(/* args */);
    ~MyCrypto();
public:
    //加密
  static int Encrypt(const BinArr &in, BinArr &out);
  //解密
  static int Decrypt(const BinArr &in, BinArr &out);
};


#endif // __H__MYCRYPTO__H__
