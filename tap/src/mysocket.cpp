#include "mysocket.h"

#include <iostream>

#ifdef _WIN32
#include <WinSock2.h>
#endif

#include "myip.h"

using namespace std;

MySocket::MySocket(/* args */)
{
}

MySocket::~MySocket()
{
}

//连接到远程socket,本地端口可选
MyConn MySocket::Connect(const MyIP *remoteip, int remoteport, ConnType type, int localport)
{
    MyConn mc;
    *mc.ip_ = *remoteip;
    mc.port_ = remoteport;
    mc.localport_ = localport;
    mc.tp_ = type;

    SOCKET_FD fd;
    switch(type){
    case ConnType::TCP:
        fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        break;
    case ConnType::UDP:
        fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        break;
    }

    mc.fd_ = fd;

    if (fd == INVALID_SOCKET)
    {
        //int er = WSAGetLastError();
        cout << "create socket failed" << endl;
        throw "create socket failed";
        return mc;
    }

    if(localport!=-1){
        sockaddr_in service;
        service.sin_family = AF_INET;
        service.sin_addr.s_addr = inet_addr("127.0.0.1");
        service.sin_port = htons(localport);

        //----------------------
        // Bind the socket.
        if (bind(fd, (SOCKADDR *)&service, sizeof(service)) == SOCKET_ERROR)
        {
            mc.Close();
            return mc;
        }
    }

    //没本地端口
    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(remoteport);
    server.sin_addr.s_addr = inet_addr(remoteip->ToString().c_str());
    if (connect(fd, (sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
    {
        cout << "connect failed" << endl;
        throw "connect socket failed";
        return mc;
    }

    

    return mc;
}

//windows需要初始化
int MySocket::InitLib()
{
    int r = 0;
#ifdef _WIN32
    WSAData wsa;
    r = ::WSAStartup(MAKEWORD(2, 2), &wsa);
    if (r!= 0)
    {
        cout << "WSAStartup error" << endl;
        return 0;
    }
#endif
    return r;
}

int MySocket::CleanLib()
{
    int r = 0;
#ifdef _WIN32
    r = WSACleanup();
#endif
    return r;
}
