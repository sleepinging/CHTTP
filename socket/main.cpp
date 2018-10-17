#include <WinSock2.h>

#include <iostream>
#include <memory>
#include <sstream>

#include "mysocketclient.h"
#include "mysocketserver.h"
#include "myhttp.h"
#include "mytools.h"

using namespace std;

int socketclienttest(string url){
    int r;
    shared_ptr<MySocketClient> psct;
    try{
        psct=make_shared<MySocketClient>(SocketType::TCP,url,80,true);
    }catch(exception& e){
        cout<<e.what()<<endl;
        return 0;
    }
    r=psct->SetRecvTimeOut(1000);
    char buf[2048]={0};
    string get="GET / HTTP/1.1\r\n";
    get+="Host: "+url+"\r\n";
    get+= "Connection: keep-alive\r\n";
    get+= "Cache-Control: max-age=0\r\n";
    get+= "Upgrade-Insecure-Requests: 1\r\n";
    get+= "DNT: 1\r\n";
    get+= "User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/68.0.3440.106 Safari/537.36\r\n";
    get+= "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n";
    get+= "Accept-Encoding: deflate\r\n";
    get+= "Accept-Language: zh-CN,zh;q=0.9\r\n";
    get+="\r\n";
    r=psct->Send(get.c_str(),get.length());
    while((r=psct->Receive(buf,2047))>0){
        cout<<buf;
    }
    //cout<<"end with "<<r<<endl;
    r=psct->DisConnect();
    return 0;
}

int socketservertest(){
    return 0;
}

int httptest(const string &url){
    auto headers=HttpReqHeaders::GenDefaultHead();
    // MyHttpGET mhg("www.zhihu.com",headers);
    // auto resp=mhg.Get();
    // cout<<(char*)&resp.Body[0]<<endl;

    MyHttpPOST mhp("www.zhihu.com",headers);
    //测试map
    map<string,string> mp={{"name","twt"},{"pwd","123"}};
    mhp.SetData(mp);
    //mhp.SetData("name=twt&pwd=123");
    auto resp=mhp.Post();
    cout<<(char*)&resp.Body[0]<<endl;

    return 0;
}

int main(int argc, char const *argv[])
{
    auto buf=readfileall("E:/temp/new 1.txt");
    //auto u=URL::ParseUrl("https://www.baidu.com:443/search?w=1&q=2");
    auto u=URL::ParseUrl("www.baidu.com");
    auto s=u.ToString();

    string url="www.baidu.com";
    if(argc>1){
        url=argv[1];
    }
    httptest(url);
    socketclienttest(url);
    
    return 0;
}
