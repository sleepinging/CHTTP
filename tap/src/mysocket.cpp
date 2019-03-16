#include "mysocket.h"

#include <string.h>

#include <iostream>

#ifdef _WIN32
#include <WinSock2.h>
#else
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#ifndef INVALID_SOCKET
#define INVALID_SOCKET (SOCKET_FD)(~0)
#endif
#ifndef SOCKET_ERROR
#define SOCKET_ERROR (-1)
#endif
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

    SOCKET_FD fd=0;
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

    if(localport!=-1){ //有本地端口
        //设置可以重用
#ifdef _WIN32
        char on = 1;
#else
        int on = 1;
#endif
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        // cout << "set opt:" << r << endl;

        sockaddr_in service;
        service.sin_family = AF_INET;
        //service.sin_addr.s_addr = inet_addr("127.0.0.1");
        service.sin_port = htons(localport);

        //----------------------
        // Bind the socket.
        if (::bind(fd, (sockaddr *)&service, sizeof(service)) == SOCKET_ERROR)
        {
            cout << "bind port:" << localport << endl;
            perror("bind failed");
            throw "bind failed";
            mc.Close();
            return mc;
        }
    }

    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(remoteport);
    server.sin_addr.s_addr = inet_addr(remoteip->ToString().c_str());
    memcpy(mc.skt_addr_, &server, sizeof(sockaddr_in));
    // if (type == ConnType::UDP)
    // {
    //     return mc;
    // }
    if (connect(fd, (sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
    {
        cout << "connect socket failed" << endl;
        throw "connect socket failed";
        return mc;
    }

    mc.conned = true;

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
