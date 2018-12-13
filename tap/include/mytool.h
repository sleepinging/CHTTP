//工具类

#if !defined(__H__MYTOOL__H__)
#define __H__MYTOOL__H__

#include <string>

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

#endif // __H__MYTOOL__H__
