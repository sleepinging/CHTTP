#include "myconn.h"

#ifdef _WIN32
// #include <WinSock2.h>
#else
#include <unistd.h>
#endif

#include "myip.h"

MyConn::MyConn(/* args */)
{
    ip_ = new MyIP();
    skt_addr_ = new sockaddr();
    sktaddrlen_ = sizeof(sockaddr);
}

MyConn::MyConn(SOCKET_FD fd, int port, MyIP *ip)
    :fd_(fd),port_(port)
{
    ip_ = new MyIP();
    skt_addr_ = new sockaddr();
    sktaddrlen_ = sizeof(sockaddr);
    *ip_ = *ip;
}

MyConn::MyConn(MyConn &&conn)
{
    ip_ = conn.ip_;
    conn.ip_ = nullptr;
    skt_addr_ = conn.skt_addr_;
    conn.skt_addr_ = nullptr;

    fd_ = conn.fd_;
#ifdef _WIN32
    fd_ = INVALID_SOCKET;
#else
    fd_ = -1;
#endif
    port_ = conn.port_;
    tp_ = conn.tp_;
    localport_ = conn.localport_;

    sktaddrlen_ = conn.sktaddrlen_;
}

MyConn &MyConn::operator=(MyConn &&conn)
{
    ip_ = conn.ip_;
    conn.ip_ = nullptr;
    skt_addr_ = conn.skt_addr_;
    conn.skt_addr_ = nullptr;

    fd_ = conn.fd_;
    conned = conn.conned;
    conn.conned = false;
    port_ = conn.port_;
    tp_ = conn.tp_;
    localport_ = conn.localport_;

    sktaddrlen_ = conn.sktaddrlen_;

    return *this;
}

MyConn::~MyConn()
{
    Close();
    delete skt_addr_;
    skt_addr_ = nullptr;
    delete ip_;
    ip_ = nullptr;
}

//设置发送超时
int MyConn::SetSendTimeOut(const int timeout)
{
#ifdef _WIN32
    char tv = timeout;
#else
    timeval tv = {timeout, 0};
#endif
    return setsockopt(fd_, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
}

//设置接收超时
int MyConn::SetRecvTimeOut(const int timeout)
{
#ifdef _WIN32
    char tv = timeout;
#else
    timeval tv = {timeout, 0};
#endif
    return setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

//读取
int MyConn::Read(char *buf, size_t len)
{
    int r = 0;
    // switch (tp_){
    // case ConnType::TCP:
        r = recv(fd_, (char *)buf, len, 0);
    //     break;
    // case ConnType::UDP:
    //     r = recvfrom(fd_, buf, len, 0, skt_addr_, &sktaddrlen_);
    // }
    return r;
}

//写入
int MyConn::Write(const char *buf, size_t len)
{
    int r = 0;
    // switch (tp_)
    // {
    // case ConnType::TCP:
        r = send(fd_, (const char *)buf, len, 0);
    //     break;
    // case ConnType::UDP:
    //     r = sendto(fd_, buf, len, 0, skt_addr_, sktaddrlen_);
    // }
    return r;
}

//关闭
int MyConn::Close(){
    int r = 0;
    if(!conned){
        return r;
    }
#ifdef _WIN32
    closesocket(fd_);
#else
    close(fd_);
#endif
    conned = false;
    return r;
}