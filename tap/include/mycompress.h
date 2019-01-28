/*
 * @Description: LZO解压缩
 * @Author: taowentao
 * @Date: 2019-01-28 13:08:31
 * @LastEditors: taowentao
 * @LastEditTime: 2019-01-28 13:15:58
 */
#ifndef __H__MYCOMPRESS__H__
#define __H__MYCOMPRESS__H__

#include <stdint.h>

class MyCompress
{
private:
    /* data */
public:
    MyCompress(/* args */);
    ~MyCompress();
    
    //压缩
    static int Compress(const unsigned char *in, uint64_t inlen, unsigned char *out, uint64_t *outlen);

    //解压
    static int DeCompress(const unsigned char *in, uint64_t inlen, unsigned char *out, uint64_t *outlen);

    static int Init();
    static int CleanUp();
};

#endif