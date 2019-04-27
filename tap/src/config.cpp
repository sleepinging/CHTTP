#include "config.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

#include "tfile.h"
#include "mystring.h"

Config* Config::is_=nullptr;

using namespace std;

Config::Config(){

}

Config::~Config(){

}

//从字符串读取配置
bool Config::LoadFromString(const std::string &str)
{
    stringstream ss(str);
    string line;
    while (getline(ss,line)){
        if(!handleline(line)){
            break;
        }
    }
    return true;
}

//处理一行,返回false停止
bool Config::handleline(const std::string &line){
    string key, value;
    vector<string> vec;
    split(line, "=", vec);
    if(vec.size()<2){
        return true;
    }
    //转小写
    key = vec[0];
    transform(vec[0].cbegin(), vec[0].cend(), key.begin(), ::tolower);
    value = vec[1];
    //transform(vec[1].cbegin(), vec[1].cend(), value.begin(), ::tolower);

    //去除空格
    trim(key);
    trim(value);

    //跳过注释
    if (
        key[0] == '#' ||
        key[0] == '\'' ||
        key[0] == ';' ||
        (key[0] == '/' && key[1] == '/'))
    {
        return true;
    }

    // cout << "key:" << key << ",value:" << value << endl;

    if(key=="serverip"){
        ServerIP = std::move(value);
        return true;
    }
    if(key=="dataport"){
        DataPort = mytrans<string, int>(std::move(value));
        return true;
    }
    if (key == "ctlport")
    {
        CTLPort = mytrans<string, int>(std::move(value));
        return true;
    }
    if (key == "mac")
    {
        MAC = std::move(value);
        return true;
    }
    if (key == "ipnet")
    {
        IPNET = std::move(value);
        return true;
    }
    if (key == "linuxtunpath")
    {
        LinuxTUNPath = std::move(value);
        return true;
    }
    if (key == "adapter_key")
    {
        Adapter_Key = std::move(value);
        return true;
    }
    if (key == "network_connections_key")
    {
        NetWork_Connections_Key = std::move(value);
        return true;
    }
    if (key == "username")
    {
        UserName = std::move(value);
        return true;
    }
    if (key == "password")
    {
        PassWord = std::move(value);
        return true;
    }

    else if (key == "end")
    {
        return false;
    }

    kvmap[key] = std::move(value);

    return true;
}

//检查正确性
bool Config::check()
{
    // if(deviceId==""){
    //     cout << "deviceid can't small than 0" << endl;
    //     return false;
    // }
    // if (ListenPort<=0){
    //     cout << "port can't small than 0" << endl;
    //     return false;
    // }
    if(ServerIP==""){
        cout << "serverip can not empty" << endl;
        return false;
    }
    if (DataPort<=0){
        cout << "DataPort can't small than 0" << endl;
        return false;
    }
    if (CTLPort <= 0)
    {
        cout << "CTLPort can't small than 0" << endl;
        return false;
    }
    if(MAC==""){
        cout << "mac can not empty" << endl;
        return false;
    }
    if (IPNET == "")
    {
        cout << "ipnet can not empty" << endl;
        return false;
    }
    if (UserName == "")
    {
        cout << "UserName can not empty" << endl;
        return false;
    }
    if (PassWord == "")
    {
        cout << "PassWord can not empty" << endl;
        return false;
    }
#ifdef _WIN32
    if (Adapter_Key == "")
    {
        cout << "Adapter_Key can not empty" << endl;
        return false;
    }
    if (NetWork_Connections_Key == "")
    {
        cout << "NetWork_Connections_Key can not empty" << endl;
        return false;
    }
#else
    if (LinuxTUNPath == "")
    {
        cout << "LinuxTUNPath can not empty" << endl;
        return false;
    }
#endif
    return true;
}

//初始化,可以是文件或者http地址
bool Config::Init(const std::string &cf)
{
    auto cfg = new Config();
    string str;
    //网络地址
    if(cf.find("http")==0){
        // Http::get(cf, &str);
        cout << "not support now" << endl;
        delete cfg;
        cfg = nullptr;
        return false;
    }
    else//文件
    {
        //可以不存在，返回空
        str = TFile::ReadAllString(cf);
    }

    if (!cfg->LoadFromString(str))
    {
        delete cfg;
        cfg = nullptr;
        return false;
    }

    if (!cfg->check()){
        delete cfg;
        cfg = nullptr;
        return false;
    }

    is_ = cfg;
    return true;
}

Config* Config::GetInstance(){
    if(!is_){
        //is_=new Config();
        cout << "you must init config first!" << endl;
        throw "you must init config first!";
    }
    return is_;
}

int Config::CleanUp(){
    delete is_;
    is_=nullptr;

    return 0;
}