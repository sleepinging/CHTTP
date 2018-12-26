//带写缓冲的socket连接

#if !defined(__H__MYBUFCONN__H__)
#define __H__MYBUFCONN__H__

#include "mydef.hpp"
#include "myconn.h"

class MySocket;

class MyIP;

template <typename T>
class Channel;

class MyBufConn
{
private:
  MySocket *skt_ = nullptr;
  Channel<BinArr> *channel_ = nullptr;
  MyConn conn_;
  /* data */
public:
    MyBufConn(const MyIP *ip,int port,ConnType tp,size_t bufsize=512);
    ~MyBufConn();
private:
  MyBufConn(/* args */);
private:
    //工作线程
  int th_work();

public:
  //读取
  int Read(char *buf, size_t len);

  //写入
  int Write(const char *buf, size_t len);
};

#endif // __H__MYBUFCONN__H__
