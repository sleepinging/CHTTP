#include "mypacket.h"

#include <iostream>

#include "mymac.h"
#include "myip.h"
#include "mytool.h"

using namespace std;

MyPacket::MyPacket(const unsigned char *data, uint32_t len)
    : data_(std::move(vector<unsigned char>(data, data + len)))
{
    //parse();
}

MyPacket::MyPacket(const char *data, uint32_t len)
    : MyPacket((const unsigned char *)data, len)
{
    //parse();
}

MyPacket::~MyPacket()
{

    //先保存一份，数据销毁之后不能获取
    auto et = getpackettype();

    //一个对象malloc可以使用delete
    delete ethlay_;
    ethlay_ = nullptr;

    switch (et)
    {
    //如果是IPv4或VLAN,继续
    case PacketType::IPv4:
    case PacketType::VLAN:
        break;

    //其它协议没有数据，不需要删除
    default:
        return;
    }

    //先保存一份，IP层数据销毁之后不能获取
    auto t = gettransprotocol();

    delete iplayer_;
    iplayer_ = nullptr;

    switch (t)
    {
    case TransProtocol::TCP:
        delete tcplater_;
        tcplater_ = nullptr;
        break;

    case TransProtocol::ICMP:
        break;

    case TransProtocol::UDP:
        delete udplayer_;
        udplayer_ = nullptr;
        break;
    }
}

MyPacket::MyPacket()
{
}

int MyPacket::parse(const unsigned char *data, uint32_t len)
{
    data_ = std::move(vector<unsigned char>(data, data + len));
    return 1;
}

int MyPacket::parse()
{
    //2层数据解析
    //ethlay_=new eth_hdr();
    //使用malloc只是分配内存，不需要初始化
    ethlay_ = (eth_hdr *)malloc(sizeof(eth_hdr));
    if (ethlay_ == nullptr)
    {
        err_ = ERR::OUTOFMEMORY;
        return -2;
    }
    auto r = parseeth();
    if (r != 0)
    {
        return r;
    }

    //3层数据解析
    //iplayer_=new ip_hdr();
    iplayer_ = (ip_hdr *)malloc(sizeof(ip_hdr));
    if (iplayer_ == nullptr)
    {
        err_ = ERR::OUTOFMEMORY;
        return -2;
    }
    r = parseip();
    if (r != 0)
    {
        return r;
    }

    //先判断4层是TCP还是UDP，然后再解析
    switch (gettransprotocol())
    {
    case TransProtocol::TCP:
        //tcplater_=new tcp_hdr();
        tcplater_ = (tcp_hdr *)malloc(sizeof(tcp_hdr));
        if (tcplater_ == nullptr)
        {
            err_ = ERR::OUTOFMEMORY;
            return -2;
        }
        parsetcp();
        break;

    case TransProtocol::UDP:
        //udplayer_=new udp_hdr();
        udplayer_ = (udp_hdr *)malloc(sizeof(udp_hdr));
        if (udplayer_ == nullptr)
        {
            err_ = ERR::OUTOFMEMORY;
            return -2;
        }
        parseudp();
        break;

    default:
        //dump(iplayer_,20,"iphead");
        err_ = ERR::UnsupportIPProtocol;
        //cout<<"Unsupport Protocol!"<<endl;
        return -1;
    }

    //解析5层数据
    parseappdata();

    //原始数据解析完毕，可以删除
    data_.clear();
    return 0;
}

int MyPacket::parseeth()
{
    copyn(ethlay_, 14);
    switch (getpackettype())
    {
    case PacketType::IPv4:
        break;

    case PacketType::VLAN:
        //VLAN帧多占4字节
        cur_ += 4;
        break;

    default:
        err_ = ERR::UnsupportETHProtocol;
        //cout<<"Unsupport Protocol!"<<endl;
        return -1;
    }
    return 0;
}

int MyPacket::parseip()
{
    copyn(iplayer_, 20);
    //IP头部长度
    auto hl = getipheadlen();
    //如果头部大于20，需要再次复制
    // if(hl>20){
    //     copyn(iplayer_+20,hl-20);
    // }
    //暂时不需要这部分内容
    cur_ += hl - 20;
    return 0;
}

int MyPacket::parsetcp()
{
    copyn(tcplater_, 20);
    //TCP头部长度
    auto hl = gettcpheadlen();
    //如果头部大于20，需要再次复制
    // if(hl>20){
    //     copyn(tcplater_+20,hl-20);
    // }
    //暂时不需要这部分内容
    cur_ += hl - 20;
    return 0;
}

int MyPacket::parseudp()
{
    copyn(udplayer_, 8);
    return 0;
}

int MyPacket::parseappdata()
{
    appdata_.reserve(data_.size() - cur_);
    appdata_.resize(appdata_.capacity());
    copy(data_.begin() + cur_, data_.end(), appdata_.begin());
    return 0;
}

void MyPacket::copyn(void *dst, uint32_t n)
{
    memcpy(dst, &data_[0] + cur_, n);
    cur_ += n;
}

TransProtocol MyPacket::gettransprotocol() const
{
    TransProtocol t = static_cast<TransProtocol>(iplayer_->protocol);
    return t;
}

void MyPacket::getappstring(std::string &ostr) const
{
    if (appdata_.size() == 0)
        return;
    ostr = string((char *)&appdata_[0]);
}

std::vector<unsigned char> &MyPacket::getappdata()
{
    return appdata_;
}

void MyPacket::getsrcmac(unsigned char mac[6]) const
{
    memcpy(mac, ethlay_->src_mac, 6);
}

const unsigned char *MyPacket::getsrcmac() const
{
    return ethlay_->src_mac;
}

void MyPacket::getdstmac(unsigned char mac[6]) const
{
    memcpy(mac, ethlay_->dst_mac, 6);
}

const unsigned char *MyPacket::getdstmac() const
{
    return ethlay_->dst_mac;
}

PacketType MyPacket::getpackettype() const
{
    auto p = (const unsigned char *)(void *)&ethlay_->eth_type;
    auto t = BigEndian::Read<unsigned short>(p);
    return (PacketType)t;
}

unsigned short MyPacket::getipheadlen() const
{
    auto d = (unsigned char *)iplayer_;
    return (d[0] & 0x0f) * 4;
}

unsigned char MyPacket::getipversion() const
{
    auto d = (unsigned char *)iplayer_;
    return d[0] & 0x0f >> 4;
}

void MyPacket::getsrcip(unsigned char ip[4]) const
{
    memcpy(ip, iplayer_->sourceIP, 4);
}

const unsigned char *MyPacket::getsrcip() const
{
    return iplayer_->sourceIP;
}

void MyPacket::getdstip(unsigned char ip[4]) const
{
    memcpy(ip, iplayer_->destIP, 4);
}

const unsigned char *MyPacket::getdstip() const
{
    return iplayer_->destIP;
}

unsigned short MyPacket::getsrcport() const
{
    unsigned char *d = nullptr;
    switch (gettransprotocol())
    {
    case TransProtocol::TCP:
        d = (unsigned char *)tcplater_;
        break;

    case TransProtocol::UDP:
        d = (unsigned char *)udplayer_;
        break;

    default:
        d = (unsigned char *)udplayer_;
    }
    return (unsigned short)d[1] + d[0] * 0x100;
}

unsigned short MyPacket::getdstport() const
{
    unsigned char *d = nullptr;
    switch (gettransprotocol())
    {
    case TransProtocol::TCP:
        d = (unsigned char *)tcplater_;
        break;

    case TransProtocol::UDP:
        d = (unsigned char *)udplayer_;
        break;

    default:
        d = (unsigned char *)udplayer_;
    }
    d += 2;
    return (unsigned short)d[1] + d[0] * 0x100;
}

unsigned short MyPacket::gettcpheadlen() const
{
    auto d = (unsigned char *)tcplater_;
    d += 12;
    return (d[0] >> 4) * 4;
}

unsigned short MyPacket::getudppacketlen() const
{
    auto d = (unsigned char *)udplayer_;
    d += 4;
    return (unsigned short)d[1] + d[0] * 0x100;
    ;
}

unsigned char MyPacket::gettcpflag() const
{
    return tcplater_->flags;
}

ostream &operator<<(ostream &co, const MyPacket &mp)
{
    //输出MAC信息
    MyMAC mac;
    mp.getdstmac(mac.data);
    // printf("dst_mac : %02x:%02x:%02x:%02x:%02x:%02x\n",
    //     mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
    co << "dst mac:" << mac.ToUp(':') << endl;
    mp.getsrcmac(mac.data);
    // printf("src_mac : %02x:%02x:%02x:%02x:%02x:%02x\n",
    //     mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
    co << "src mac:" << mac.ToUp(':') << endl;

    //输出IP信息
    MyIP ip;
    mp.getsrcip(ip.data);
    // printf("src_ip : %d.%d.%d.%d\n",
    //     ip[0],ip[1],ip[2],ip[3]);
    co << "src ip:" << ip.ToString() << endl;
    mp.getdstip(ip.data);
    // printf("dst_ip : %d.%d.%d.%d\n",
    //     ip[0],ip[1],ip[2],ip[3]);
    co << "dst ip:" <<ip.ToString() << endl;

    //输出传输层信息
    co << "Protocol:";
    if (mp.gettransprotocol() == TransProtocol::TCP)
    {
        co << "TCP" << endl;
    }
    else if (mp.gettransprotocol() == TransProtocol::UDP)
    {
        co << "UDP" << endl;
        //dump((char*)mp.udplayer_,20,"udp.data");
    }
    co << "src port:" << mp.getsrcport() << endl;
    co << "dst port:" << mp.getdstport() << endl;

    //输出应用层信息
    // co<<"APP DATA:"<<endl;
    // string str;
    // mp.getappstring(str);
    // co<<str<<endl;
    //dump((char*)&mp.appdata_[0],mp.appdata_.size(),"appdata.data");

    return co;
}

MyPacket::ERR MyPacket::getlasterr()
{
    return err_;
}