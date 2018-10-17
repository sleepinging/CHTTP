#ifndef __H__MYTOOLS__H__
#define __H__MYTOOLS__H__

#include <stdlib.h>

#include <vector>
#include <string>
#include <iostream>
#include <sstream>

//splite the string
void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c);

//delete blank chars at head and end
std::string& trim(std::string &s);

//get public ip
std::string getpublicip(std::string api);

//dump data from memory
void dump(const void *p, size_t len, const char *filename);

//读取文件
std::vector<unsigned char> readfileall(const char* const filename);

//获取文件大小
size_t getfilesize(const char* const filename);

//string转uint64
uint64_t stringtouint64(const std::string& str);

//将mac字节数组转字符串
std::string mactostring(const unsigned char mac[6]);

//将字符串转化为MAC
bool stringtomac(const std::string& str,unsigned char mac[6]);

//将IP字节数组转字符串
std::string iptostring(const unsigned char ip[4]);

//大端数据转换为小端数据
int big2small(unsigned short v);

//利用KMP算法，验证字符串P是否为T的子字符串，
//如果是返回P第一个字母对应T的字符的位置，否则返回-1
int KmpCmp(const char *T,int Tlen,const char *P,int Plen);

/*
 * 编码一个url
 * 结果需要先分配输入的3倍内存
 */
void urlencode(const char url[],size_t inlen,char* strout,size_t* lenout);

/*
 * 解码url
 * 输出字符需要先分配同样大的内存
 */
void urldecode(const char* instr,size_t inlen,char outstr[],size_t *outlen);

//字符串与数字转换
template <typename S,typename D>
D mytrans(S s){
    D d;
    std::stringstream ss;
    ss<<s;
    ss>>d;
    return d;
}

#endif
