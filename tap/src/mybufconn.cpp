#include "mybufconn.h"

#include <iostream>
#include <thread>

#include "mysocket.h"
#include "channel.hpp"
#include "myip.h"

using namespace std;

MyBufConn::MyBufConn(/* args */)
{
}

MyBufConn::MyBufConn(const MyIP *ip, int port, ConnType tp, size_t bufsize)
{
    skt_ = new MySocket();
    conn_ = new MyConn();
    while(1){//重复连接到成功
        try{
            *conn_ = skt_->Connect(ip, port, tp);
            break;
        }
        catch (...)
        {
            cout << "connect to "<<ip->ToString() <<":"<<port<<" failed,retry..." << endl;
            this_thread::sleep_for(chrono::seconds(1));
        }
    }
    channel_ = new Channel<BinArr>(bufsize);
    thread(&MyBufConn::th_work, this).detach();
}

MyBufConn::~MyBufConn()
{
    delete conn_;
    conn_ = nullptr;
    delete skt_;
    skt_ = nullptr;
    delete channel_;
    channel_ = nullptr;
}

//工作线程
int MyBufConn::th_work()
{
    int r = 0;
    for (;;)
    {
        auto ba = channel_->pop();
        r=conn_->Write((const char*)&ba[0], ba.size());
        if (r < 0)
        {
            cout << "write failed:"<<r<<" reconnect..." << endl;
            conn_->ReConnect();
        }
    }
    return r;
}

//读取
int MyBufConn::Read(char *buf, size_t len){
    int r = 0;
    if (len == 0)
    {
        return r;
    }
    r = conn_->Read(buf, len);
    if(r<0){
        cout << "read failed:" << r << " reconnect..." << endl;
        conn_->ReConnect();
    }
    return r;
}

//写入
int MyBufConn::Write(const char *buf, size_t len){
    int r = len;
    if(len==0){
        return r;
    }
    channel_->push({buf, buf + len});
    // r = conn_->Write(buf, len);
    return r;
}