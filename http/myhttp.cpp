#include "myhttp.h"

#include <algorithm>

#include "mysocketclient.h"
#include "mytools.h"

using namespace std;

/*
URL
*/

URL URL::ParseUrl(const std::string &url){
    URL u;
    //解析协议
    auto index=url.find("://",0);
    if(index!=string::npos){//写了协议
        auto ptl=string(url.begin(),url.begin()+index);
        //转小写
        transform(ptl.begin(),ptl.end(),ptl.begin(),::tolower);
        if(ptl=="http"){
            u.Port=80;
            u.Protocol=HTTPTYPE::HTTP;
        }else if(ptl=="https"){
            u.Port=443;
            u.Protocol=HTTPTYPE::HTTPS;
        }
        else{
            throw "UnSupported Protocol";
        }
        index+=3;
    }else{//没写协议,默认http
        index=0;
        u.Protocol=HTTPTYPE::HTTP;
    }
    //解析主机+端口
    auto si=index;
    index=url.find("/",index);
    if(index!=string::npos){//找到/
        u.Host=string(url.begin()+si,url.begin()+index);
        //解析端口
        si=u.Host.find(":",si);
        if(si!=string::npos){
            auto ps=string(u.Host.begin()+si+1,u.Host.end());
            u.Port=stringtouint64(ps);
            u.Host.resize(si);
        }
    }
    else{//没找到/,默认全是host
        u.Host=string(url.begin()+si,url.end());
        //解析端口
        si=u.Host.find(":",si);
        if(si!=string::npos){
            auto ps=string(u.Host.begin()+si+1,u.Host.end());
            u.Port=stringtouint64(ps);
            u.Host.resize(si);
        }
        return u;
    }
    //index+=1;
    si=index;
    index=url.find("?",index);
    if(index!=string::npos){//带参数
        u.Path=string(url.begin()+si,url.begin()+index);
    }else{//不带参数
        u.Path=string(url.begin()+si,url.end());
        return u;
    }
    index+=1;
    u.Param=string(url.begin()+index,url.end());
    return u;
}

//整个URL转成字符串
std::string URL::ToString(){
    string url("");
    switch(Protocol){
        case HTTPTYPE::HTTP:
            url.append("http://");
            url.append(Host);
            if(Port!=80) url.append(":"+to_string(Port));
            break;
        case HTTPTYPE::HTTPS:
            url.append("https://");
            url.append(Host);
            if(Port!=443) url.append(":"+to_string(Port));
            break;
    }
    url.append(Path);
    if(Param!="") url.append("?"+Param);

    return url;
}

//将请求部分转成字符串
std::string URL::getstring(){
    string url;
    url.append(Path);
    if(Param!="") url.append("?"+Param);
    return url;
}

/*
HttpResponse
*/

HttpResponse HttpResponse::Parse(const std::string& res){
    HttpResponse resp;
    vector<string> kv;
    string k,v;
    //找第一行
    auto idx=res.find("\r\n",0);
    if(idx==string::npos){//没有状态
        throw "no status";
    }
    resp.HeadersLen=idx+2;
    auto fl=string(res.begin(),res.begin()+idx);//第一行，包含状态
    kv.clear();
    SplitString(fl,kv," ");
    if(kv.size()<2){//没有协议版本或者状态码
        throw "no status code";
    }
    resp.headers.StautsCode=mytrans<string,int>(kv[1]);

    //解析头部
    auto nres=string(res.begin()+idx+2,res.end());
    idx=nres.find("\r\n\r\n",idx);
    if(idx==string::npos){//可能没有头部
        throw "no header";
    }
    resp.HeadersLen+=idx+4;
    auto bidx=idx;
    auto shd=string(nres.begin(),nres.begin()+idx);
    vector<string> hd;
    kv.clear();
    SplitString(shd,hd,"\r\n");
    for(const auto& h:hd){
        idx=h.find(":",0);
        if(idx==string::npos) continue;
        k=string(h.begin(),h.begin()+idx);
        v=string(h.begin()+idx+2,h.end());
        k=trim(k);
        v=trim(v);
        resp.headers.headers[k].emplace_back(v);
        if(k=="Content-Length"){
            resp.headers.ContentLen=mytrans<string,int>(v);
        }else if(k=="Location"){//跳转
            resp.headers.Location=v;
        }
    }
    resp.Body.resize(nres.end()-(nres.begin()+bidx+4));
    copy(nres.begin()+bidx+4,nres.end(),resp.Body.begin());
    
    return resp;
}

/*
GET
*/

MyHttpGET::MyHttpGET(const std::string& url,const HttpReqHeaders &headers){
    auto ui=URL::ParseUrl(url);
    url_=ui;
    headers_=headers;
    if(url_.Protocol!=HTTPTYPE::HTTP) throw "UnSupported Protocol";
    msc_=new MySocketClient(SocketType::TCP,url_.Host,url_.Port,true);
}

MyHttpGET::MyHttpGET(const URL &url,const HttpReqHeaders &headers){
    url_=url;
    headers_=headers;
    if(url_.Protocol!=HTTPTYPE::HTTP) throw "UnSupported Protocol";
    msc_=new MySocketClient(SocketType::TCP,url_.Host,url_.Port,true);
}

MyHttpGET::~MyHttpGET(){
    cout<<"delete"<<endl;
    delete msc_;
    msc_=nullptr;
}

//设置允许几次跳转,-1无限制
void MyHttpGET::SetRedirect(int n){
    redirect_=n;
}

//将请求和头部信息组合成字符串
std::string MyHttpGET::GenData(){
    string str("");
    str.append("GET ");
    str.append(url_.getstring());
    str.append(" HTTP/1.1\r\n");
    str.append("Host: "+url_.Host+"\r\n");
    for(const auto& hm:headers_.Header){
        str.append(hm.first).append(": ");
        str.append(hm.second).append("\r\n");
    }
    str.append("\r\n");
    return str;
}

HttpResponse MyHttpGET::Get(int sendtimeout,int recvtimeout){
    msc_->SetSendTimeOut(sendtimeout);
    msc_->SetRecvTimeOut(recvtimeout);
    auto str=GenData();
    msc_->Send(str.c_str(),str.length());
    char buf[2048]={0};
    //先至少获取头部信息
    msc_->Receive(buf,2047);
    //dump(buf,2048,"1.data");
    //解析头部
    HttpResponse resp =HttpResponse::Parse(buf);
    //获取剩余的主体长度
    auto restlen=resp.headers.ContentLen-resp.Body.size();
    if(restlen>0){
        char* pb=new char[restlen];
        msc_->Receive(pb,restlen,true);
        auto l=resp.Body.size();
        resp.Body.resize(resp.Body.size()+restlen);
        copy(pb,pb+restlen,resp.Body.begin()+l);
        delete pb;
    }
    return resp;
}

/*
POST
*/

MyHttpPOST::MyHttpPOST(const std::string& url,const std::map<std::string,std::string> &data,const HttpReqHeaders &headers){
    auto ui=URL::ParseUrl(url);
    url_=ui;
    SetData(data);
    headers_=headers;
    if(url_.Protocol!=HTTPTYPE::HTTP) throw "UnSupported Protocol";
    msc_=new MySocketClient(SocketType::TCP,url_.Host,url_.Port,true);
}

MyHttpPOST::MyHttpPOST(const std::string& url,const HttpReqHeaders &headers){
    auto ui=URL::ParseUrl(url);
    url_=ui;
    headers_=headers;
    if(url_.Protocol!=HTTPTYPE::HTTP) throw "UnSupported Protocol";
    msc_=new MySocketClient(SocketType::TCP,url_.Host,url_.Port,true);
}

MyHttpPOST::MyHttpPOST(const URL &url,const std::map<std::string,std::string> &data,const HttpReqHeaders &headers){
    url_=url;
    SetData(data);
    headers_=headers;
    if(url_.Protocol!=HTTPTYPE::HTTP) throw "UnSupported Protocol";
    msc_=new MySocketClient(SocketType::TCP,url_.Host,url_.Port,true);
}

MyHttpPOST::MyHttpPOST(const URL &url,const HttpReqHeaders &headers){
    url_=url;
    headers_=headers;
    if(url_.Protocol!=HTTPTYPE::HTTP) throw "UnSupported Protocol";
    msc_=new MySocketClient(SocketType::TCP,url_.Host,url_.Port,true);
}

MyHttpPOST::~MyHttpPOST(){
    delete msc_;
    msc_=nullptr;
}

//设置post内容
int MyHttpPOST::SetData(const std::map<std::string,std::string> &data){
    if(data.size()<1) return 0;
    string str=data.begin()->first+"="+data.begin()->second;
    data_.insert(data_.begin(),str.begin(),str.end());
    if(data.size()>1){
        //for_each(++data.begin(),data.end(),[](const auto& itm){
        //C++14以上支持上面的简单写法
        for_each(++data.begin(),data.end(),[&](const decltype(*data.begin()) & itm){
            str="&"+itm.first+"="+itm.second;
            data_.insert(data_.end(),str.begin(),str.end());
        });
    }
    return 0;
}

//设置post内容
int MyHttpPOST::SetData(const std::string &data){
    data_.insert(data_.begin(),data.begin(),data.end());
    return 0;
}

//设置post内容
int MyHttpPOST::SetData(std::vector<unsigned char> &data){
    data_=data;
    return 0;
}

//将请求,头部信息和数据组合成
std::vector<unsigned char> MyHttpPOST::GenData(){
    string str("");
    str.append("POST ");
    str.append(url_.getstring());
    str.append(" HTTP/1.1\r\n");
    str.append("Host: "+url_.Host+"\r\n");
    for(const auto& hm:headers_.Header){
        str.append(hm.first).append(": ");
        str.append(hm.second).append("\r\n");
    }
    str.append("\r\n");
    vector<unsigned char> data;
    data.resize(str.length());
    copy(str.begin(),str.end(),data.begin());
    data.insert(data.end(),data_.begin(),data_.end());
    return data;
}

HttpResponse MyHttpPOST::Post(int sendtimeout,int recvtimeout){
    msc_->SetSendTimeOut(sendtimeout);
    msc_->SetRecvTimeOut(recvtimeout);
    auto data=GenData();
    /**/
    string str;
    str.insert(str.begin(),data.begin(),data.end());
    /**/
    msc_->Send(&data[0],data.size());
    char buf[2048]={0};
    //先至少获取头部信息
    msc_->Receive(buf,2047);
    //dump(buf,2048,"1.data");
    //解析头部
    HttpResponse resp =HttpResponse::Parse(buf);
    //获取剩余的主体长度
    auto restlen=resp.headers.ContentLen-resp.Body.size();
    if(restlen>0){
        char* pb=new char[restlen];
        msc_->Receive(pb,restlen,true);
        auto l=resp.Body.size();
        resp.Body.resize(resp.Body.size()+restlen);
        copy(pb,pb+restlen,resp.Body.begin()+l);
        delete pb;
    }
    return resp;
}