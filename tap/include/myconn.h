//连接类

#if !defined(__H__MYCONN__H__)
#define __H__MYCONN__H__

#ifdef _WIN32
    #include <WinSock2.h>
    #include <Windows.h>
    #define SOCKET_FD SOCKET
#else
    #include <sys/socket.h>
    #define SOCKET_FD int
#endif

enum class ConnType:unsigned short
{
    TCP=6,
    UDP=17
};

class MyIP;

class sockaddr;

class MyConn
{
private:
  SOCKET_FD fd_;
  sockaddr* skt_addr_ = nullptr;
  int sktaddrlen_ = 0;

public:
  //IP,端口和类型缓存
  int port_;
  MyIP *ip_ = nullptr;
  ConnType tp_;

  int localport_;

  bool conned = false;

public:
  MyConn();
  MyConn(const MyConn&) =delete;
  MyConn(MyConn&& conn);
  MyConn(SOCKET_FD fd,int port,MyIP* ip);
  ~MyConn();
  friend class MySocket;
  MyConn& operator = (const MyConn &)=delete;
  MyConn &operator=(MyConn && conn);

  //读取
  int Read(char *buf, size_t len);

  //写入
  int Write(const char *buf, size_t len);

  //设置发送超时
  int SetSendTimeOut(const int timeout);

  /**
   * @description: 重连
   * @param 最大次数，-1无限制
   * @return: 
   */
  int ReConnect(int max=-1);

  //设置接收超时
  int SetRecvTimeOut(const int timeout);

  //关闭
  int Close();
};

#endif // __H__MYCONN__H__
