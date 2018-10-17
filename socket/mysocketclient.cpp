#include "mysocketclient.h"

#include <iostream>

using namespace std;

MySocketClient::MySocketClient(){
    if(!MySocket::IsWSAStartup){
        WSADATA wsadata;
        if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0){
            throw "WSAStartup() error";
        }
        MySocket::IsWSAStartup=true;
    }
}

MySocketClient::~MySocketClient(){

}

MySocketClient::MySocketClient(const SocketType type,const std::string &host,const int port,const bool con):
    host_(host),port_(port),type_(type){
        MySocketClient();
        if(con) Connect();
}

//连接
int MySocketClient::Connect(){
    if(Connected()) return socketfd_;
    SOCKET r=INVALID_SOCKET;
	SOCKADDR_IN servAddr;
    switch(type_){
        case SocketType::TCP:
            socketfd_ = socket(PF_INET, SOCK_STREAM, 0);
        break;
        
        case SocketType::UDP:
            socketfd_ = socket(PF_INET, SOCK_DGRAM, 0);
        break;
    }
    
	if (socketfd_ == INVALID_SOCKET){
        cout<<"socket() error"<<endl;
        return socketfd_;
    }
 
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
    hostent *phst=gethostbyname(host_.c_str());
    in_addr * iddr=(in_addr*)phst->h_addr;
	servAddr.sin_addr.s_addr = iddr->s_addr;  //回送地址，访问本地
	servAddr.sin_port = htons(port_); //端口号
	//客户端套接字通过目的地址建立连接
	if ((r=connect(socketfd_, (SOCKADDR*)&servAddr, sizeof(servAddr)))){
        cout<<"conect() error"<<endl;
        return r;
    }
    return socketfd_;
}

//连接
int MySocketClient::Connect(SocketType type,const std::string &host,const int port){
    host_=host;
    type_=type;
    port_=port;
    return Connect();
}

bool MySocketClient::Connected(){
    return socketfd_!=INVALID_SOCKET;
}

//断开连接
int MySocketClient::DisConnect(){
    auto r=closesocket(socketfd_);
    if(r==0) socketfd_=INVALID_SOCKET;
    return r;
}

//设置发送超时
int MySocketClient::SetSendTimeOut(const int timeout){
    return setsockopt(socketfd_, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout,sizeof(int));
}

//设置接收超时
int MySocketClient::SetRecvTimeOut(const int timeout){
    return setsockopt(socketfd_, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout,sizeof(int));;
}

//发送数据,最多sz字节,是否强制必须sz字节,返回已发送字节数
int MySocketClient::Send(const void* data,const size_t datalen,const bool must){
    if(!must){
        return write(data,datalen);
    }
    //发送总数
    size_t tn=0;
    int t=0;
    for(;tn<datalen;tn+=t){
        t=write(data,datalen);
        if(t<=0){
            return t;
        }
    }
    return tn;
}

//接收数据,最多sz字节,是否强制必须sz字节,返回已接收字节数,0表示服务器正常退出,否则调用WSAGetLastError查看
int MySocketClient::Receive(void* data,const size_t datalen,const bool must){
    if(!must){
        return read(data,datalen);
    }
    //接收总数
    size_t tn=0;
    int t=0;
    for(;tn<datalen;tn+=t){
        t=read(data,datalen);
        if(t<=0){
            return t;
        }
    }
    return 0;
}

int MySocketClient::read(void* buf,const size_t len){
    return recv(socketfd_,(char*)buf,len,0);
}

int MySocketClient::write(const void* buf,const size_t len){
    return send(socketfd_,(const char*)buf,len,0);
}