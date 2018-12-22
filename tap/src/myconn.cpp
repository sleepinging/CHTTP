#include "myconn.h"

//#include <WinSock2.h>

#include "myip.h"

MyConn::MyConn(/* args */)
{
    ip_ = new MyIP();
}

MyConn::MyConn(SOCKET_FD fd, int port, MyIP *ip)
    :fd_(fd),port_(port)
{
    ip_ = new MyIP();
    *ip_ = *ip;
}

MyConn::~MyConn()
{
    Close();
    delete ip_;
}

//设置发送超时
int MyConn::SetSendTimeOut(const int timeout)
{
    return setsockopt(fd_, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout, sizeof(int));
}

//设置接收超时
int MyConn::SetRecvTimeOut(const int timeout)
{
    return setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(int));
}

//读取
int MyConn::Read(char *buf, size_t len)
{
    int r = 0;
    r=recv(fd_, (char *)buf, len, 0);
    return r;
}

//写入
int MyConn::Write(const char *buf, size_t len)
{
    int r = 0;
    r=send(fd_, (const char *)buf, len, 0);
    return r;
}

//关闭
int MyConn::Close(){
    int r = 0;
#ifdef _WIN32
    closesocket(fd_);
#else
    close(fd_);
#endif
    if(r==0){
        fd_ = -1;
    }
    return r;
}