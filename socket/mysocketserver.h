#ifndef __H__MYSOCKETSERVER__H__
#define __H__MYSOCKETSERVER__H__

#include <WinSock2.h>

#include <string>
#include <set>
#include <functional>

#include "mysocket.h"

//accept处理函数,返回false表示不需要,关闭连接
using AcceptHandler=std::function<bool(SOCKET fd,const std::string &ip,const int port)>;

class MySocketServer{
public:
    MySocketServer();

    //类型,监听端口,是否立即监听,可能抛出异常
    MySocketServer(const SocketType type,const int port,const bool ls=false);

    //开始监听端口
    int StartListen();

    //开始监听端口
    int StartListen(const int port);

    //处理accpet响应
    void SetAccpetHandler(AcceptHandler fp);

    //断开一个客户端的连接
    int CloseClient(SOCKET confd);

    //停止监听
    int StopListen();
    
    ~MySocketServer();

    //接收数据,最多sz字节,是否强制必须sz字节,返回已接收字节数,0表示服务器正常退出,否则调用WSAGetLastError查看
    static int ReadSocket(SOCKET s,void* data,const size_t datalen,const bool must=false);

    //发送数据,最多sz字节,是否强制必须sz字节,返回已发送字节数
    static int WriteSocket(SOCKET s,const void* data,const size_t datalen,const bool must=false);

private:
    int port_;
    SocketType type_;
    SOCKET socketfd_;

    //客户端连接表
    std::set<SOCKET> confd_;

    //accept响应函数
    AcceptHandler accepthandler_;
};

#endif // __H__MYSOCKETSERVER__H__
