#ifndef __H__MYSOCKET__H__
#define __H__MYSOCKET__H__

enum class SocketType
{
    TCP = 6,
    UDP = 17
};

class MySocket{
public:
    //是否已经调用过WSAStartup
    static bool IsWSAStartup;

    //设置读取超时

    //设置写入超时
private:
    //socketfd
    
};

#endif