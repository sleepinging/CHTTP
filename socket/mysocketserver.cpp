#include "mysocketserver.h"

MySocketServer::MySocketServer(){

}

//类型,监听端口,是否立即监听,可能抛出异常
MySocketServer::MySocketServer(const SocketType type,const int port,const bool ls){

}

MySocketServer::~MySocketServer(){
    
}

//开始监听端口
int MySocketServer::StartListen(){

}

//开始监听端口
int MySocketServer::StartListen(const int port){

}

//处理accpet响应
void MySocketServer::SetAccpetHandler(AcceptHandler fp){
    accepthandler_=fp;
}

//断开一个客户端的连接
int MySocketServer::CloseClient(SOCKET confd){

}

//停止监听
int MySocketServer::StopListen(){

}

//接收数据,最多sz字节,是否强制必须sz字节,返回已接收字节数,0表示服务器正常退出,否则调用WSAGetLastError查看
int MySocketServer::ReadSocket(SOCKET s,void* data,const size_t datalen,const bool must){

}

//发送数据,最多sz字节,是否强制必须sz字节,返回已发送字节数
int MySocketServer::WriteSocket(SOCKET s,const void* data,const size_t datalen,const bool must){

}