//工具类

#if !defined(__H__MYTOOL__H__)
#define __H__MYTOOL__H__

#include <string>
#include <vector>

//获取TAP设备的ID
std::string GetTAPComponentId();

//安全删除
#define SafeDelete(p) do{delete(p);(p)=nullptr;}while(0)
// template <typename T>
// void SafeDelete(T p){
//     delete p;
//     p = nullptr;
// }

//将1个字符转为数字(16进制)
unsigned char atohex(char c);

//将1个字符转为数字(10进制)
unsigned char atodec(char c);

//将一个字节转为2个字符(16进制小写)
void hextoa(unsigned char h, char buf[2]);

//将一个字节转为2个字符(16进制大写)
void hextoA(unsigned char h, char buf[2]);

//分割字符串
void split(const std::string &s, const std::string &c, std::vector<std::string> &v);

//去除首尾空格
std::string &trim(std::string &s);

#endif // __H__MYTOOL__H__
