//工具类

#if !defined(__H__MYTOOL__H__)
#define __H__MYTOOL__H__

#include <string>
#include <initializer_list>

//获取注册表项的值
std::string GetRegValue(const std::string& dir,const std::string name);

//修改注册表的值
int SetRegValueString(const std::string &dir, const std::string &name, const std::string &value);

//获取TAP设备的ID
std::string GetTAPComponentId();

//执行系统命令,默认输出
int ExecCmd(std::initializer_list<std::string> cmds,bool show=true);

//安全删除
#define SafeDelete(p) do{delete(p);(p)=nullptr;}while(0)
// template <typename T>
// void SafeDelete(T p){
//     delete p;
//     p = nullptr;
// }

#endif // __H__MYTOOL__H__
