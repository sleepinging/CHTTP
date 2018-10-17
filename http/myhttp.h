#ifndef __H__MYHTTP__H__
#define __H__MYHTTP__H__

#include <string>
#include <map>
#include <vector>

class MySocketClient;

enum class HTTPTYPE{
    HTTP,
    HTTPS
};

//URL信息
class URL{
public:
    //协议
    HTTPTYPE Protocol=HTTPTYPE::HTTP;
    //主机名(域名)
    std::string Host;
    //端口
    int Port=80;
    //路径
    std::string Path="/";
    //参数
    std::string Param="";
public:
    //从url字符串中获取url信息,可能抛出异常
    static URL ParseUrl(const std::string &url);
    //整个URL转成字符串
    std::string ToString();
    friend class MyHttpGET;
    friend class MyHttpPOST;
private:
    //将请求部分转成字符串
    std::string getstring();
};

//发送的http头部
class HttpReqHeaders{
public:
    // std::string Host;
    // std::string Refer;
    // std::string Accept;
    std::map <std::string,std::string> Header;

public:
    //常用的默认头部
    static HttpReqHeaders GenDefaultHead(){
        HttpReqHeaders headers;
        headers.Header["Accept"]="text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8";
        return headers;
    }
};

//回应的http头部
class HttpRespHeaders{
public:
    //返回状态码
    int StautsCode;
    //body长度
    size_t ContentLen=0;
    //跳转地址
    std::string Location;
    //其它内容
    std::map<std::string,std::vector<std::string> > headers;
};

//HTTP回应
class HttpResponse{
public:
    //头部
    HttpRespHeaders headers;
    //头部长度
    size_t HeadersLen;
    //主体
    std::vector<unsigned char> Body;
public:
    friend class MyHttpGET;
    friend class MyHttpPOST;
private:
    //解析回应字符串,可能有异常
    static HttpResponse Parse(const std::string& res);
    //继续解析
    //int ContinueParse(const std::string& res);
};

class MyHttpGET{
public:
    //如果地址格式错误会有异常
    MyHttpGET(const std::string& url,const HttpReqHeaders &headers);
    MyHttpGET(const URL &url,const HttpReqHeaders &headers);
    ~MyHttpGET();
    //设置允许几次跳转,-1无限制
    void SetRedirect(int n);
    //请求,发送和接收超时,-1为无限制
    HttpResponse Get(int sendtimeout=-1,int recvtimeout=-1);
public:
private:
    //将请求和头部信息组合成字符串
    std::string GenData();
private:
    //允许几次跳转
    int redirect_=0;
    MySocketClient* msc_=nullptr;
    URL url_;
    HttpReqHeaders headers_;
};

class MyHttpPOST{
public:
    //如果地址格式错误会有异常
    MyHttpPOST(const std::string& url,const std::map<std::string,std::string> &data,const HttpReqHeaders &headers);
    MyHttpPOST(const std::string& url,const HttpReqHeaders &headers);
    MyHttpPOST(const URL &url,const std::map<std::string,std::string> &data,const HttpReqHeaders &headers);
    MyHttpPOST(const URL &url,const HttpReqHeaders &headers);
    ~MyHttpPOST();
    //请求,发送和接收超时,-1为无限制
    HttpResponse Post(int sendtimeout=-1,int recvtimeout=-1);
    //设置post内容
    int SetData(const std::map<std::string,std::string> &data);
    //设置post内容
    int SetData(const std::string &data);
    //设置post内容
    int SetData(std::vector<unsigned char> &data);
private:
    //将请求,头部信息和数据组合成
    std::vector<unsigned char> GenData();
private:
    MySocketClient* msc_=nullptr;
    //请求的数据
    std::vector<unsigned char> data_;
    URL url_;
    HttpReqHeaders headers_;
};

#endif