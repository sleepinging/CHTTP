//简单的socket封装

#if !defined(__H__MYSOCKET__H__)
#define __H__MYSOCKET__H__

#include "myconn.h"

class MyIP;

class MySocket
{
private:
    /* data */
public:
    MySocket(/* args */);
    ~MySocket();
public:
    //连接到远程socket,本地端口可选
  static MyConn Connect(const MyIP *remoteip, int remoteport, ConnType type, int localport = -1);
  //static Listerner Listen();

  //windows需要初始化
  static int InitLib();
  static int CleanLib();
};

#endif // __H__MYSOCKET__H__
