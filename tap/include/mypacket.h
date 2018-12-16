//数据包分析和构造类

#if !defined(__H__MYPACKET__H__)
#define __H__MYPACKET__H__

#include <vector>
#include <string>

#include <netdef.hpp>

class MyPacket
{
public:
    //构造时直接解析
    MyPacket(const unsigned char *data, uint32_t len);
    //构造时直接解析
    MyPacket(const char *data, uint32_t len);
    ~MyPacket();

    //解析数据,需要在构造函数就传入数据
    int parse();

    //只分配内存不初始化
    MyPacket();

    //解析数据，需要之前分配空间
    int parse(const unsigned char *data, uint32_t len);

    //获取应用层数据
    std::vector<unsigned char> &getappdata();

    //获取数据链路层
    //获取源MAC
    void getsrcmac(unsigned char mac[6]) const;

    //返回源MAC的指针,可能会被删除
    const unsigned char *getsrcmac() const;

    //获取目的MAC
    void getdstmac(unsigned char mac[6]) const;

    //返回目的MAC的指针,可能会被删除
    const unsigned char *getdstmac() const;

    //获取数据帧类型
    PacketType getpackettype() const;

    //获取网络层数据
    //获取IP版本,4 or 6
    unsigned char getipversion() const;

    //获取IP头部长度
    unsigned short getipheadlen() const;

    //获取源IP
    void getsrcip(unsigned char ip[4]) const;

    //返回源IP的指针,可能会被删除
    const unsigned char *getsrcip() const;

    //获取目的IP
    void getdstip(unsigned char ip[4]) const;

    //返回目的IP的指针,可能会被删除
    const unsigned char *getdstip() const;

    //获取传输层数据
    //获取传输层类型
    TransProtocol gettransprotocol() const;

    //获取源端口
    unsigned short getsrcport() const;

    //获取目的端口
    unsigned short getdstport() const;

    //获取TCP层数据
    //获取TCP头长度
    unsigned short gettcpheadlen() const;

    //获取TCP的6位标识
    //前2位为0，后6位分别是UAPRSF
    unsigned char gettcpflag() const;

    //获取UDP层数据
    //获取UDP数据包长度
    unsigned short getudppacketlen() const;

    //将应用层数据转化为string,很可能不完整
    void getappstring(std::string &ostr) const;

    //输出信息
    friend std::ostream &operator<<(std::ostream &co, const MyPacket &mp);

  public:
    enum class ERR
    {
        UnsupportETHProtocol,
        UnsupportIPProtocol,
        OUTOFMEMORY,
        Other
    };

    //获取最近一次的错误信息
    MyPacket::ERR getlasterr();

  private:
    //解析数据链路层
    int parseeth();

    //解析网络层
    int parseip();

    //解析传输层
    //解析TCP
    int parsetcp();

    //解析UDP
    int parseudp();

    //解析应用层数据
    int parseappdata();

    //将数据包从游标开始的N个字节复制到dst，同时游标增加N
    void copyn(void *dst, uint32_t n);

  private:
    //全部数据
    std::vector<unsigned char> data_;

    //应用数据
    std::vector<unsigned char> appdata_;

    //数据链路层
    eth_hdr *ethlay_ = nullptr;

    //网络层
    ip_hdr *iplayer_ = nullptr;

    //应用层
    //TCP
    tcp_hdr *tcplater_ = nullptr;

    //UDP
    udp_hdr *udplayer_ = nullptr;

    //解析数据时的游标
    uint32_t cur_ = 0;

    //最近的错误
    MyPacket::ERR err_;
};


#endif // __H__MYPACKET__H__
