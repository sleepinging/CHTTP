#include "task.h"

#include <thread>
#include <iostream>

#include "mybufconn.h"
#include "config.h"
#include "myip.h"

using namespace std;

//心跳,返回0成功
int DetachHeartBeat(int sec)
{
    int r = 0;
    static MyBufConn *mbc = nullptr;
    auto cfg = Config::GetInstance();
    const auto &sip = cfg->ServerIP;
    auto port = cfg->CTLPort;

    MyIP ip(sip);
    while (1)//直到连接成功
    { 
        cout << "connect to " << ip.ToString()<<":"<<port << flush;
        try
        {
            mbc = new MyBufConn(&ip, port, ConnType::TCP);
            cout << " success" << endl;
            break;
        }
        catch (...)
        {
            r = -1;
            cout << " failed,retry after 10 second..." << endl;
            this_thread::sleep_for(chrono::seconds(10));
        }
    }
    //连接成功
    static const unsigned char heart_packet[] = {1};
    thread([=]() {
        for (;; this_thread::sleep_for(chrono::seconds(sec)))
        {
            mbc->Write((const char*)heart_packet, 1);
        }
    }).detach();

    return r;
}