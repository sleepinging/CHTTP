#include "mymac.h"

using namespace std;

MyMAC::MyMAC(/* args */)
{
}

MyMAC::~MyMAC()
{
}

//从字符串解析
int Parse(const std::string &mac){
    if(mac.length()<12){
        return -1;
    }
    if(mac.length()==12){
        
    }
    return 0;
}