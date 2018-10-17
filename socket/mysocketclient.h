#ifndef __H__MYSOCKETCLIENT__H__
#define __H__MYSOCKETCLIENT__H__

#include <WinSock2.h>

#include <string>

#include "mysocket.h"

class MySocketClient
{
public:
    //可能抛出异常
    MySocketClient();

    //类型,服务器host,服务器端口,是否立即连接,可能抛出异常
    MySocketClient(const SocketType type,const std::string &host,const int port,const bool con=false);

    //连接
    int Connect();

    //连接
    int Connect(SocketType type,const std::string &host,const int port);

    //是否已经连接
    bool Connected();

    //断开连接
    int DisConnect();

    //设置发送超时
    int SetSendTimeOut(const int timeout);

    //设置接收超时
    int SetRecvTimeOut(const int timeout);

    //发送数据,最多sz字节,是否强制必须sz字节,返回已发送字节数
    int Send(const void* data,const size_t datalen,const bool must=false);

    //接收数据,最多sz字节,是否强制必须sz字节,返回已接收字节数,0表示服务器正常退出,否则调用WSAGetLastError查看
    int Receive(void* data,const size_t datalen,const bool must=false);

    ~MySocketClient();

private:
    int read(void* buf,const size_t len);
    int write(const void* buf,const size_t len);

private:
    std::string host_;
    int port_;
    SocketType type_;
    SOCKET socketfd_=INVALID_SOCKET;

};

#endif